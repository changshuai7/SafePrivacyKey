package com.shuai.privacy.example.app;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.shuai.privacy.PrivacyKeyGenerator;


public class MainActivity extends AppCompatActivity {

    private EditText mEtKeyType;
    private Button mBtnGetKey;
    private TextView mTvKeyShow;
    private TextView mTvSha1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        mEtKeyType = findViewById(R.id.et_key_type);
        mBtnGetKey = findViewById(R.id.btn_get_key);
        mTvKeyShow = findViewById(R.id.tv_key_show);
        mTvSha1 = findViewById(R.id.tv_sha1);

        mBtnGetKey.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String type = mEtKeyType.getText().toString();
                String key = PrivacyKeyGenerator.getPrivacyKey(MainActivity.this, type, BuildConfig.DEBUG);


                mTvKeyShow.setText(key);

            }
        });
        //获取当前应用签名的sha1值
        String sha1 = SignUtils.getSha1Value(MainActivity.this);
        mTvSha1.setText("\n当前应用签名的sha1值:\n" + sha1);

    }

}