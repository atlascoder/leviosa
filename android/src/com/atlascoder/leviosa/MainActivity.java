package com.atlascoder.leviosa;

import com.espressif.iot.esptouch.EsptouchResult;
import com.espressif.iot.esptouch.EsptouchTask;
import com.espressif.iot.esptouch.IEsptouchResult;
import com.espressif.iot.esptouch.IEsptouchTask;

import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiInfo;
import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.math.BigInteger;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.List;

public class MainActivity extends QtActivity{

    EsptouchTask mTask;
    IEsptouchResult mResult;
    Object mEsptouchLock = new Object();
    boolean mIsRunning = false;

    public String runEsptouch(String apSsid, String apBssid, String apPass){
        if(mIsRunning) return "cancelled";
        mIsRunning = true;
        Log.d("ET", "begin");
        synchronized (mEsptouchLock){
            Log.d("ET", "starting");
            mTask = new EsptouchTask(getWifiConnectedSsidAscii(apSsid), apBssid, apPass, this);
            mResult = mTask.executeForResult();
            Log.d("ET", "finished");
            mIsRunning = false;
            if(mResult.isSuc() && !mResult.isCancelled()){
                String result = mResult.getBssid() + "/" + mResult.getInetAddress().getHostAddress();
                Log.d("ET", "returned: " + result);
                return result;
            }
            else {
                Log.d("ET", "cancelled");
                return "cancelled";
            }
        }
    }

    public String getSsid(){
        WifiManager wifiMgr = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        WifiInfo wifiInfo = wifiMgr.getConnectionInfo();
        String _ssid = wifiInfo.getSSID();
        String ssid = _ssid;
        if (_ssid.startsWith("\"") && _ssid.endsWith("\"")) {
            ssid = _ssid.substring(1, _ssid.length() - 1);
        }
        return ssid;
    }

    public String getBssid(){
        WifiManager wifiMgr = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        WifiInfo wifiInfo = wifiMgr.getConnectionInfo();
        return wifiInfo.getBSSID();
    }

    public String getWlanIP(){
        WifiManager wifiMgr = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        WifiInfo wifiInfo = wifiMgr.getConnectionInfo();
        byte[] bytes = BigInteger.valueOf(wifiInfo.getIpAddress()).toByteArray();
        String addr = "0.0.0.0";
        try{
            addr = InetAddress.getByAddress(bytes).toString();
        }
        catch (UnknownHostException e){

        }
        return addr;
    }

    public void interruptEsptouch(){
        if(mTask != null){
            Log.d("ET", "interrupting");
            mTask.interrupt();
        }
    }

    public String getWifiConnectedSsidAscii(String ssid) {
        final long timeout = 100;
        final long interval = 20;
        String ssidAscii = ssid;

        WifiManager wifiManager = (WifiManager) getApplicationContext()
                .getSystemService(Context.WIFI_SERVICE);
        wifiManager.startScan();

        boolean isBreak = false;
        long start = System.currentTimeMillis();
        do {
            try {
                Thread.sleep(interval);
            } catch (InterruptedException ignore) {
                isBreak = true;
                break;
            }
            List<ScanResult> scanResults = wifiManager.getScanResults();
            for (ScanResult scanResult : scanResults) {
                if (scanResult.SSID != null && scanResult.SSID.equals(ssid)) {
                    isBreak = true;
                    try {
                        Field wifiSsidfield = ScanResult.class
                                .getDeclaredField("wifiSsid");
                        wifiSsidfield.setAccessible(true);
                        Class<?> wifiSsidClass = wifiSsidfield.getType();
                        Object wifiSsid = wifiSsidfield.get(scanResult);
                        Method method = wifiSsidClass
                                .getDeclaredMethod("getOctets");
                        byte[] bytes = (byte[]) method.invoke(wifiSsid);
                        ssidAscii = new String(bytes, "ISO-8859-1");
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;
                }
            }
        } while (System.currentTimeMillis() - start < timeout && !isBreak);

        return ssidAscii;
    }



}
