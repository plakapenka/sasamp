package com.liverussia.launcher.fragment;

import android.content.Intent;
import android.os.Bundle;

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
import com.liverussia.launcher.activity.ActivitySupportedServerSelection;
import com.liverussia.launcher.activity.ForumActivity;
import com.liverussia.launcher.activity.MainActivity;
import com.liverussia.launcher.activity.dialogs.SelectServerDialog;
import com.liverussia.launcher.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.enums.BillingParameters;
import com.liverussia.launcher.enums.ServerInfo;
import com.liverussia.launcher.enums.StorageElements;
import com.liverussia.launcher.model.Servers;
import com.liverussia.launcher.storage.Storage;
import com.liverussia.launcher.ui.adapters.DonateServicesItemListAdapter;
import com.liverussia.launcher.utils.Validator;

//TODO сделать загрузку картинок доната с сервака как в рулетке
//TODO Скачивать все картинки в main activity
public class DonateFragment extends Fragment implements View.OnClickListener, ActivitySupportedServerSelection {
	
	private Animation animation;
	private MainActivity mainActivity;
	
	private EditText nicknameField;
    private EditText emailField;
    private EditText donateSumField;
    private TextView selectServerButton;
    private GridView donateServicesPanel;

    private Servers selectedServer;

    public DonateFragment(MainActivity mainActivity) {
        this.mainActivity = mainActivity;
    }
	
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View inflate = inflater.inflate(R.layout.fragment_donate, container, false);
		
		nicknameField = inflate.findViewById(R.id.nick_input);
        emailField = inflate.findViewById(R.id.email_input);
        donateSumField = inflate.findViewById(R.id.donate_sum_input);
        donateServicesPanel = inflate.findViewById(R.id.donate_services_view);

        animation = AnimationUtils.loadAnimation(getContext(), R.anim.button_click);

        TextView rechargeButton = inflate.findViewById(R.id.rechargeButton);
        rechargeButton.setOnClickListener(this);

        selectServerButton = inflate.findViewById(R.id.btnSelectServer);
        selectServerButton.setOnClickListener(this);

        initUserData();
        addDonateServices();
		
        return inflate;
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.rechargeButton:
                v.startAnimation(animation);
                performRechargeButtonAction();
                break;
            case R.id.btnSelectServer:
                v.startAnimation(animation);
                performSelectServerButtonAction();
                break;
            default:
                break;
        }
    }

    private void performRechargeButtonAction() {
        if (!isValidInputs()) {
            return;
        }

        String email = emailField.getText().toString();
        String nickname = nicknameField.getText().toString();
        String donateSum = donateSumField.getText().toString();

        Storage.addProperty(StorageElements.EMAIL.getValue(), email, this.getActivity());

        Intent billingIntent = new Intent(this.getContext(), ForumActivity.class);
        billingIntent.putExtra(BillingParameters.NICKNAME.getName(), nickname);
        billingIntent.putExtra(BillingParameters.EMAIL.getName(), email);
        billingIntent.putExtra(BillingParameters.DONATE_SUM.getName(), donateSum);
        billingIntent.putExtra(BillingParameters.SERVER_SELECTED.getName(), selectedServer.getServerID());

        startActivity(billingIntent);
    }

    private boolean isValidInputs() {
        return Validator.isValidNickname(nicknameField.getText().toString(), this.getActivity())
                && Validator.isValidEmail(emailField.getText().toString(), this.getActivity())
                && Validator.isValidDonateSum(donateSumField.getText().toString(), this.getActivity())
                && Validator.isValidSelectedServer(selectedServer, this.getActivity());
    }

    //TODO FragmentManager
    private void performSelectServerButtonAction() {
        SelectServerDialog.display(this.getFragmentManager(), this);
    }

    private void initUserData() {
        String nickname = Storage.getProperty(StorageElements.NICKNAME.getValue(), this.getActivity());
        String email = Storage.getProperty(StorageElements.EMAIL.getValue(), this.getActivity());

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
