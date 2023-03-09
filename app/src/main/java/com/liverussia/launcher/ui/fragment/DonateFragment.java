package com.liverussia.launcher.ui.fragment;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;

import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.TextView;

import androidx.fragment.app.Fragment;

import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AnimationUtils;
import android.view.animation.Animation;
import android.view.LayoutInflater;

import com.liverussia.cr.R;
import com.liverussia.launcher.config.Config;
import com.liverussia.launcher.domain.messages.ErrorMessage;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.ui.activity.ActivitySupportedServerSelection;
import com.liverussia.launcher.ui.activity.BillingActivity;
import com.liverussia.launcher.ui.activity.MainActivity;
import com.liverussia.launcher.ui.dialogs.ReCaptchaDialog;
import com.liverussia.launcher.ui.dialogs.SelectServerDialog;
import com.liverussia.launcher.async.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.domain.enums.BillingParameters;
import com.liverussia.launcher.domain.enums.StorageElements;
import com.liverussia.launcher.async.dto.response.Servers;
import com.liverussia.launcher.storage.Storage;
import com.liverussia.launcher.ui.adapters.DonateServicesItemListAdapter;
import com.liverussia.launcher.utils.Validator;

public class DonateFragment extends Fragment implements View.OnClickListener, ActivitySupportedServerSelection {

    private final static String CONTACTS_VIEW_ACTION = "android.intent.action.VIEW";

	private Animation animation;
	private MainActivity mainActivity;
	
	private EditText nicknameField;
    private EditText emailField;
    private EditText donateSumField;
    private TextView selectServerButton;
    private GridView donateServicesPanel;
    private CheckBox iAmNotRobotCheckBox;

    private ActivityService activityService;
    private Servers selectedServer;
    private String captchaToken;

    public DonateFragment(MainActivity mainActivity) {
        this.mainActivity = mainActivity;
    }
	
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View inflate = inflater.inflate(R.layout.fragment_donate, container, false);

        activityService = new ActivityServiceImpl();
		
		nicknameField = inflate.findViewById(R.id.nick_input);
        emailField = inflate.findViewById(R.id.email_input);
        donateSumField = inflate.findViewById(R.id.donate_sum_input);
        donateServicesPanel = inflate.findViewById(R.id.donate_services_view);

        animation = AnimationUtils.loadAnimation(getContext(), R.anim.button_click);

        TextView rechargeButton = inflate.findViewById(R.id.rechargeButton);
        rechargeButton.setOnClickListener(this);

        TextView rechargeQiwiButton = inflate.findViewById(R.id.rechargeQiwiButton);
        rechargeQiwiButton.setOnClickListener(this);

        iAmNotRobotCheckBox = inflate.findViewById(R.id.i_am_not_robot_checkbox);
        iAmNotRobotCheckBox.setOnClickListener(this);

        selectServerButton = inflate.findViewById(R.id.btnSelectServer);
        selectServerButton.setOnClickListener(this);

        initUserData();
        addDonateServices();
		
        return inflate;
    }

    private void performIAmNotRobotCheckBoxAction() {
        if (iAmNotRobotCheckBox.isChecked()) {
            iAmNotRobotCheckBox.setChecked(false);
            ReCaptchaDialog reCaptchaDialog = new ReCaptchaDialog(this.getActivity());
            reCaptchaDialog.setOnDialogCloseListener(this::performCaptchaSuccessAction);
            reCaptchaDialog.createDialog();
        } else {
            activityService.showMessage(ErrorMessage.CAPTCHA_NOT_PASSED.getText(), this.getActivity());
        }
    }

    public void performCaptchaSuccessAction(String token) {
        captchaToken = token;
        iAmNotRobotCheckBox.setChecked(true);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.rechargeButton:
                v.startAnimation(animation);
                performRechargeButtonAction();
                break;
            case R.id.rechargeQiwiButton:
                v.startAnimation(animation);
                performRechargeQiwiButtonAction();
                break;
            case R.id.btnSelectServer:
                v.startAnimation(animation);
                performSelectServerButtonAction();
                break;
            case R.id.i_am_not_robot_checkbox:

                try {
                    performIAmNotRobotCheckBoxAction();
                } catch (Exception exception) {
                    activityService.showBigMessage(exception.getMessage(), this.getActivity());
                }
                break;
            default:
                break;
        }
    }

    private void performRechargeQiwiButtonAction() {
        if (!isValidInputs()) {
            return;
        }

        startActivity(new Intent(CONTACTS_VIEW_ACTION, Uri.parse(createUri())));
    }

    private String createUri() {

        String nickname = nicknameField.getText().toString();
        String email = emailField.getText().toString();
        String serverId = selectedServer.getServerID();
        String serverName = selectedServer.getname();
        String sum = donateSumField.getText().toString();
        String captcha = captchaToken;

        return Config.createBillingUri(serverId, serverName, sum, nickname, email, captcha);
    }

    private void performRechargeButtonAction() {
        if (!isValidInputs()) {
            return;
        }

        String email = emailField.getText().toString();
        String nickname = nicknameField.getText().toString();
        String donateSum = donateSumField.getText().toString();

        Storage.addProperty(StorageElements.EMAIL, email, this.getActivity());
        Storage.addProperty(StorageElements.NICKNAME, nickname, this.getActivity());

        Intent billingIntent = new Intent(this.getContext(), BillingActivity.class);
        billingIntent.putExtra(BillingParameters.NICKNAME.getName(), nickname);
        billingIntent.putExtra(BillingParameters.EMAIL.getName(), email);
        billingIntent.putExtra(BillingParameters.DONATE_SUM.getName(), donateSum);
        billingIntent.putExtra(BillingParameters.SERVER_SELECTED_ID.getName(), selectedServer.getServerID());
        billingIntent.putExtra(BillingParameters.SERVER_SELECTED_NAME.getName(), selectedServer.getname());
        billingIntent.putExtra(BillingParameters.CAPTCHA.getName(), captchaToken);

        startActivity(billingIntent);
    }

    private boolean isValidInputs() {
        return Validator.isValidNickname(nicknameField.getText().toString(), this.getActivity())
                && Validator.isValidEmail(emailField.getText().toString(), this.getActivity())
                && Validator.isValidDonateSum(donateSumField.getText().toString(), this.getActivity())
                && Validator.isValidSelectedServer(selectedServer, this.getActivity())
                && Validator.isValidCaptchaToken(captchaToken, this.getActivity());
    }

    //TODO FragmentManager
    private void performSelectServerButtonAction() {
        SelectServerDialog.display(this.getFragmentManager(), this);
    }

    private void initUserData() {
        String nickname = Storage.getProperty(StorageElements.NICKNAME, this.getActivity());
        String email = Storage.getProperty(StorageElements.EMAIL, this.getActivity());

        nicknameField.setText(nickname);
        emailField.setText(email);
    }

    @Override
    public void setSelectedServer(Servers server) {
        this.selectedServer = server;
        selectServerButton.setCompoundDrawables(null, null, null, null);
        selectServerButton.setText(server.getname().toUpperCase());
    }

    private void addDonateServices() {
        ServerImagesResponseDto possiblePrizesInfoResponseDto = mainActivity.getDonateServicesResponseDto();
        if (possiblePrizesInfoResponseDto == null) {
            return;
        }
        DonateServicesItemListAdapter adapter = new DonateServicesItemListAdapter(this.getActivity(), possiblePrizesInfoResponseDto.getUrls());
        donateServicesPanel.setAdapter(adapter);
    }

    public void addDonateServices(ServerImagesResponseDto possiblePrizesInfoResponseDto) {
        DonateServicesItemListAdapter adapter = new DonateServicesItemListAdapter(this.getActivity(), possiblePrizesInfoResponseDto.getUrls());
        donateServicesPanel.setAdapter(adapter);
    }
}
