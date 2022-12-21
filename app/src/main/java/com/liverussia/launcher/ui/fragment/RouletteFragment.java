package com.liverussia.launcher.ui.fragment;

import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageButton;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.RecyclerView;
import androidx.viewpager2.widget.CompositePageTransformer;
import androidx.viewpager2.widget.MarginPageTransformer;
import androidx.viewpager2.widget.ViewPager2;

import com.liverussia.cr.R;
import com.liverussia.launcher.ui.activity.MainActivity;
import com.liverussia.launcher.ui.dialogs.AuthenticationDialog;
import com.liverussia.launcher.ui.dialogs.PrizeDialog;
import com.liverussia.launcher.async.task.SpinRouletteAsyncRestCall;
import com.liverussia.launcher.async.task.UpdateTokensAsyncRestCall;
import com.liverussia.launcher.async.task.UpdateUserInfoAsyncRestCall;
import com.liverussia.launcher.domain.RouletteItem;
import com.liverussia.launcher.ui.domain.ExpandableHeightGridView;
import com.liverussia.launcher.async.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.async.dto.response.PrizeInfoResponseDto;
import com.liverussia.launcher.async.dto.response.SpinRouletteResponseDto;
import com.liverussia.launcher.domain.enums.ServerInfo;
import com.liverussia.launcher.domain.enums.StorageElements;
import com.liverussia.launcher.domain.inscriptions.Inscriptions;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.AuthenticationService;
import com.liverussia.launcher.service.RouletteAnimationService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.service.impl.AuthenticationServiceImpl;
import com.liverussia.launcher.service.impl.RouletteAnimationServiceImpl;
import com.liverussia.launcher.storage.Storage;
import com.liverussia.launcher.ui.adapters.PossiblePrizesItemListAdapter;
import com.liverussia.launcher.ui.adapters.RouletteUIPanelAdapter;

import org.apache.commons.lang3.StringUtils;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

public class RouletteFragment extends Fragment implements View.OnClickListener, DialogInterface.OnDismissListener {
    private final static int COUNT_OF_VISIBLE_ELEMENTS_IN_ROULETTE_PANEL = 3;
    private final static int FIRST_ROULETTE_ELEMENT_IN_FIRST_SPIN = 1;

    private final static int ZERO = 0;
    private final static String EMPTY_BALANCE = "0";

    private Animation animation;

    private ActivityService activityService;
    private RouletteAnimationService rouletteAnimationService;
    private AuthenticationService authenticationService;
    private MainActivity mainActivity;

    private List<RouletteItem> rouletteItems;

    private ViewPager2 rouletteUIPanel;

    private ImageButton logoutButton;
    private TextView spinRouletteButton;

    private boolean isSpinRequestProcessing;

    private ConstraintLayout authenticatedUserRoulettePanel;
    private TextView serverNameTextView;
    private TextView usernameTextView;
    private TextView balanceTextView;
    private ExpandableHeightGridView possiblePrizesPanel;

    public RouletteFragment(MainActivity mainActivity) {
        this.mainActivity = mainActivity;
    }

    @Override
    public View onCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        animation = AnimationUtils.loadAnimation(getContext(), R.anim.button_click);

        activityService = new ActivityServiceImpl();
        rouletteAnimationService = new RouletteAnimationServiceImpl(this);
        authenticationService = new AuthenticationServiceImpl(getActivity());

        View inflate = inflater.inflate(R.layout.roulette_activity, container, false);

        super.onCreate(savedInstanceState);

        rouletteUIPanel = inflate.findViewById(R.id.viewPager);
        authenticatedUserRoulettePanel = inflate.findViewById(R.id.authenticated_user_roulette_panel);
        possiblePrizesPanel = inflate.findViewById(R.id.possible_prizes_view);
        possiblePrizesPanel.setExpanded(true);

        logoutButton = inflate.findViewById(R.id.logout_btn);
        logoutButton.setOnClickListener(this);

        spinRouletteButton = inflate.findViewById(R.id.spin_roulette_btn);
        spinRouletteButton.setOnClickListener(this);

        serverNameTextView = inflate.findViewById(R.id.server_name_text_view);
        usernameTextView = inflate.findViewById(R.id.username_text_view);
        balanceTextView = inflate.findViewById(R.id.balance_text_view);

        isSpinRequestProcessing = false;

        configureAuthenticatedPanel();
        configureSlider();
        addPossiblePrizes();


        return inflate;
    }

    @Override
    public void onStop() {
        rouletteAnimationService.cancelAnimation();
        super.onStop();
    }

    @Override
    public void onDestroy() {
        rouletteAnimationService.cancelAnimation();
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {

        if (isRouletteSpinning()) {
            return;
        }

        switch (v.getId()) {
            case R.id.spin_roulette_btn:
                v.startAnimation(animation);
                performSpinButtonAction();
                break;
            case R.id.logout_btn:
                performLogoutButtonAction();
                break;
            default:
                break;
        }
    }

    @Override
    public void onDismiss(DialogInterface dialog) {
        if (!authenticationService.isAccessTokenHealth()) {
            return;
        }

        fillAuthenticationPanelFields();
    }

    private void performSpinButtonAction() {
        if (authenticationService.isAccessTokenHealth()) {
            spinRoulette();
            return;
        }

        if (authenticationService.isRefreshTokenHealth()) {
            UpdateTokensAsyncRestCall updateTokensAsyncRestCall = new UpdateTokensAsyncRestCall(getActivity());
            updateTokensAsyncRestCall.setOnAsyncSuccessListener(this::performSpinButtonAction);
            updateTokensAsyncRestCall.setOnAsyncCriticalErrorListener(this::performLogoutButtonAction);
            updateTokensAsyncRestCall.refreshTokens(true);
            return;
        }

        openAuthenticationDialog();
    }

    private void performLogoutButtonAction() {
        authenticationService.logout();
        authenticatedUserRoulettePanel.setVisibility(View.GONE);
        logoutButton.setVisibility(View.GONE);
    }

    private void openAuthenticationDialog() {
        AuthenticationDialog.display(this, activityService);
    }


    public void openPrizeDialog(PrizeInfoResponseDto prizeInfo) {
        PrizeDialog.display(getFragmentManager(), prizeInfo);
    }

    private void spinRoulette() {
        isSpinRequestProcessing = true;
        SpinRouletteAsyncRestCall spinRouletteAsyncRestCall = new SpinRouletteAsyncRestCall(getActivity());
        spinRouletteAsyncRestCall.setOnAsyncSuccessListener(this::configureRouletteBeforeSpin);
        spinRouletteAsyncRestCall.setOnAsyncCriticalErrorListener(this::connectionRefusedAction);
        spinRouletteAsyncRestCall.spinRoulette();
    }

    private void connectionRefusedAction() {
        isSpinRequestProcessing = false;
    }

    private void configureRouletteBeforeSpin(SpinRouletteResponseDto spinRouletteResponseDto) {

        setBalanceTextView(spinRouletteResponseDto.getBalance());

        int countElementsInOneSpin = Integer.parseInt(spinRouletteResponseDto.getCountElementsInOneSpin());
        long duration = Long.parseLong(spinRouletteResponseDto.getSpinDurationInMillis());
        PrizeInfoResponseDto prizeInfo = spinRouletteResponseDto.getPrizeInfo();

        List<RouletteItem> newItems = getRouletteItemsFromSpinRouletteResponseDto(spinRouletteResponseDto);
        rouletteItems.addAll(newItems);

        if (rouletteUIPanel.getCurrentItem() == FIRST_ROULETTE_ELEMENT_IN_FIRST_SPIN) {
            spinRouletteFirstTime(countElementsInOneSpin, duration, prizeInfo);
            return;
        }

        spinRouletteNonFirstTime(countElementsInOneSpin, duration, prizeInfo);
    }

    private List<RouletteItem> getRouletteItemsFromSpinRouletteResponseDto(SpinRouletteResponseDto spinRouletteResponseDto) {
        return Optional.ofNullable(spinRouletteResponseDto.getBase64Images())
                .orElse(Collections.emptyList())
                .stream()
                .map(RouletteItem::new)
                .collect(Collectors.toList());
    }


    private void spinRouletteFirstTime(int countElementsInOneSpin, long duration, PrizeInfoResponseDto prizeInfo) {

        int rouletteStartItemIndex = rouletteUIPanel.getCurrentItem();

        addRouletteItemsIntoSlider();

        rouletteUIPanel.setCurrentItem(rouletteStartItemIndex, false);

        int rouletteEndItemIndex = rouletteUIPanel.getCurrentItem()
                + countElementsInOneSpin;

        isSpinRequestProcessing = false;

        rouletteAnimationService.setCurrentItem(
                rouletteUIPanel,
                rouletteEndItemIndex,
                duration,
                prizeInfo
        );
    }

    private void spinRouletteNonFirstTime(int countElementsInOneSpin, long duration, PrizeInfoResponseDto prizeInfo) {
        int rouletteStartItemIndex = rouletteUIPanel.getCurrentItem();

        addRouletteItemsIntoSlider();

        rouletteUIPanel.setCurrentItem(rouletteStartItemIndex, false);

        int rouletteEndItemIndex = rouletteUIPanel.getCurrentItem()
                + countElementsInOneSpin;

        isSpinRequestProcessing = false;

        rouletteAnimationService.setCurrentItem(
                rouletteUIPanel,
                rouletteEndItemIndex,
                duration,
                prizeInfo
        );
    }

    private void configureSlider() {

        rouletteItems = new ArrayList<>();

        addStartingRouletteItems();
        addRouletteItemsIntoSlider();

        rouletteUIPanel.setCurrentItem(FIRST_ROULETTE_ELEMENT_IN_FIRST_SPIN, false);

        rouletteUIPanel.setOffscreenPageLimit(COUNT_OF_VISIBLE_ELEMENTS_IN_ROULETTE_PANEL);

        rouletteUIPanel.setClipToPadding(false);
        rouletteUIPanel.setClipChildren(false);

        rouletteUIPanel.getChildAt(ZERO).setOverScrollMode(RecyclerView.OVER_SCROLL_NEVER);

        rouletteUIPanel.setUserInputEnabled(false);

        CompositePageTransformer compositePageTransformer = new CompositePageTransformer();
        compositePageTransformer.addTransformer(new MarginPageTransformer(4));

        rouletteUIPanel.setPageTransformer(compositePageTransformer);

    }

    private void addRouletteItemsIntoSlider() {
        RouletteUIPanelAdapter rouletteUIPanelAdapter = new RouletteUIPanelAdapter(rouletteItems);
        rouletteUIPanel.setAdapter(rouletteUIPanelAdapter);
    }

    private void addStartingRouletteItems() {
        rouletteItems.add(new RouletteItem(R.drawable.roulette_lamborgini_huracan_item));
        rouletteItems.add(new RouletteItem(R.drawable.roulette_money_item));
        rouletteItems.add(new RouletteItem(R.drawable.roulette_silver_vip_item));
    }

    private void configureAuthenticatedPanel() {
        if (!authenticationService.isRefreshTokenHealth()) {
            authenticatedUserRoulettePanel.setVisibility(View.GONE);
            logoutButton.setVisibility(View.GONE);
            authenticationService.logout();
            return;
        }

        if (!authenticationService.isAccessTokenHealth()) {
            UpdateTokensAsyncRestCall updateTokensAsyncRestCall = new UpdateTokensAsyncRestCall(this.getActivity());
            updateTokensAsyncRestCall.setOnAsyncSuccessListener(this::fillAuthenticationPanelFields);
            updateTokensAsyncRestCall.setOnAsyncNotCriticalErrorListener(this::fillAuthenticationPanelFields);
            updateTokensAsyncRestCall.setOnAsyncCriticalErrorListener(this::performLogoutButtonAction);

            updateTokensAsyncRestCall.refreshTokens(false);
            return;
        }

        UpdateUserInfoAsyncRestCall updateUserInfoAsyncRestCall = new UpdateUserInfoAsyncRestCall(this.getActivity());
        updateUserInfoAsyncRestCall.setOnAsyncSuccessListener(this::fillAuthenticationPanelFields);
        updateUserInfoAsyncRestCall.setOnAsyncNotCriticalErrorListener(this::fillAuthenticationPanelFields);
        updateUserInfoAsyncRestCall.setOnAsyncCriticalErrorListener(this::performLogoutButtonAction);

        updateUserInfoAsyncRestCall.updateUserInfo();
    }

    private void fillAuthenticationPanelFields() {
        authenticatedUserRoulettePanel.setVisibility(View.VISIBLE);
        logoutButton.setVisibility(View.VISIBLE);

        String username = Storage.getProperty(StorageElements.AUTHENTICATED_NICKNAME.getValue(), this.getActivity());
        String balance = Storage.getProperty(StorageElements.USER_BALANCE.getValue(), this.getActivity());
        String serverName = Storage.getProperty(StorageElements.AUTHENTICATED_SERVER.getValue(), this.getActivity());

        ServerInfo serverInfo = ServerInfo.ofName(serverName);

        if (serverInfo != null) {
            int serverBackgroundResource = serverInfo.getResource();
            serverNameTextView.setBackgroundResource(serverBackgroundResource);
        }

        setBalanceTextView(balance);
        usernameTextView.setText(username);
    }

    private void setBalanceTextView(String balance) {
        if (StringUtils.isBlank(balance)) {
            balance = EMPTY_BALANCE;
        }

        String balanceText = Inscriptions.createBalanceInscription(balance);

        balanceTextView.setText(balanceText);
    }

    private void addPossiblePrizes() {
        ServerImagesResponseDto possiblePrizesInfoResponseDto = mainActivity.getPossiblePrizesInfoResponseDto();
        if (possiblePrizesInfoResponseDto == null) {
            return;
        }
        PossiblePrizesItemListAdapter adapter = new PossiblePrizesItemListAdapter(this.getActivity(), possiblePrizesInfoResponseDto.getUrls());
        possiblePrizesPanel.setAdapter(adapter);
    }

    public void addPossiblePrizes(ServerImagesResponseDto possiblePrizesInfoResponseDto) {
        PossiblePrizesItemListAdapter adapter = new PossiblePrizesItemListAdapter(this.getActivity(), possiblePrizesInfoResponseDto.getUrls());
        possiblePrizesPanel.setAdapter(adapter);
    }

    public boolean isRouletteSpinning() {
        //TODO Вот тут npe
        return rouletteAnimationService.isAnimationRunning() || isSpinRequestProcessing;
    }
}
