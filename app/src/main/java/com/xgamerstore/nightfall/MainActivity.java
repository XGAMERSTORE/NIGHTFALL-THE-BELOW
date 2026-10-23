package com.xgamerstore.nightfall;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.graphics.Color;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.Window;
import android.webkit.ConsoleMessage;
import android.webkit.RenderProcessGoneDetail;
import android.webkit.WebChromeClient;
import android.webkit.WebResourceError;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.io.IOException;
import java.util.Collections;

public final class MainActivity extends Activity {
    private WebView gameView;
    private boolean immersiveModePending;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        launchGameSafely();
    }

    private void launchGameSafely() {
        try {
            startGameView();
        } catch (Throwable error) {
            android.util.Log.e("NightfallGame", "Could not start game", error);
            showFailure("Spuštění WebView selhalo: " + error.getClass().getSimpleName()
                    + (error.getMessage() == null ? "" : " · " + error.getMessage()));
        }
    }

    private void startGameView() {
        gameView = new WebView(this);
        gameView.setBackgroundColor(0xFF090C10);

        WebSettings settings = gameView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setDatabaseEnabled(true);
        settings.setLoadWithOverviewMode(true);
        settings.setUseWideViewPort(true);
        settings.setSupportZoom(false);
        settings.setBuiltInZoomControls(false);
        settings.setMediaPlaybackRequiresUserGesture(false);
        settings.setAllowFileAccess(false);
        settings.setAllowContentAccess(false);
        settings.setJavaScriptCanOpenWindowsAutomatically(false);
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_NEVER_ALLOW);

        gameView.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onConsoleMessage(ConsoleMessage message) {
                android.util.Log.d("NightfallGame", message.message() + " ("
                        + message.sourceId() + ":" + message.lineNumber() + ")");
                return true;
            }
        });
        gameView.setWebViewClient(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O
                ? new ModernGameWebViewClient() : new GameWebViewClient());
        setContentView(gameView);
        applyImmersiveModeWhenReady();
        gameView.loadUrl("https://appassets.androidplatform.net/assets/index.html");
    }

    private class GameWebViewClient extends WebViewClient {
        @Override
        public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
            if (request.isForMainFrame()) {
                showFailure("Nepodařilo se otevřít herní svět. " + error.getDescription());
            }
        }

        @Override
        public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
            Uri uri = request.getUrl();
            String path = uri.getPath();
            if ("appassets.androidplatform.net".equals(uri.getHost())
                    && "https".equals(uri.getScheme())
                    && path != null
                    && path.startsWith("/assets/")
                    && !path.contains("..")) {
                String assetPath = path.substring("/assets/".length());
                String mime = assetPath.endsWith(".html") ? "text/html"
                        : assetPath.endsWith(".js") ? "application/javascript"
                        : assetPath.endsWith(".css") ? "text/css"
                        : assetPath.endsWith(".webp") ? "image/webp"
                        : assetPath.endsWith(".png") ? "image/png"
                        : "application/octet-stream";
                try {
                    String encoding = mime.startsWith("text/") || mime.equals("application/javascript")
                            ? "UTF-8" : null;
                    return new WebResourceResponse(mime, encoding, 200, "OK",
                            Collections.singletonMap("Access-Control-Allow-Origin", "*"),
                            MainActivity.this.getAssets().open(assetPath));
                } catch (IOException ignored) {
                    return new WebResourceResponse("text/plain", "UTF-8", 404, "Not found",
                            Collections.emptyMap(), null);
                }
            }
            return super.shouldInterceptRequest(view, request);
        }
    }

    @TargetApi(Build.VERSION_CODES.O)
    private final class ModernGameWebViewClient extends GameWebViewClient {
        @Override
        public boolean onRenderProcessGone(WebView view, RenderProcessGoneDetail detail) {
            showFailure("3D vykreslování se zastavilo. Zkus hru znovu spustit.");
            if (view.getParent() instanceof ViewGroup) {
                ((ViewGroup) view.getParent()).removeView(view);
            }
            view.destroy();
            if (gameView == view) gameView = null;
            return true;
        }
    }

    private void showFailure(String message) {
        if (isFinishing()) return;
        LinearLayout screen = new LinearLayout(this);
        screen.setOrientation(LinearLayout.VERTICAL);
        screen.setGravity(Gravity.CENTER);
        screen.setPadding(32, 32, 32, 32);
        screen.setBackgroundColor(Color.rgb(9, 12, 16));

        TextView title = new TextView(this);
        title.setText("NIGHTFALL se nepodařilo spustit");
        title.setTextColor(Color.WHITE);
        title.setTextSize(22);
        title.setGravity(Gravity.CENTER);
        screen.addView(title);

        TextView detail = new TextView(this);
        String diagnostics = message + "\n\nAndroid " + Build.VERSION.RELEASE
                + " (API " + Build.VERSION.SDK_INT + ")\n"
                + Build.MANUFACTURER + " " + Build.MODEL;
        detail.setText(diagnostics);
        detail.setTextColor(Color.LTGRAY);
        detail.setTextSize(13);
        detail.setGravity(Gravity.CENTER);
        detail.setPadding(0, 16, 0, 20);
        screen.addView(detail);

        Button copy = new Button(this);
        copy.setText("KOPÍROVAT DIAGNOSTIKU");
        copy.setOnClickListener(v -> {
            ClipboardManager clipboard = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
            if (clipboard != null) {
                clipboard.setPrimaryClip(ClipData.newPlainText("NIGHTFALL diagnostics", diagnostics));
                copy.setText("DIAGNOSTIKA ZKOPÍROVÁNA");
            }
        });
        screen.addView(copy);

        Button retry = new Button(this);
        retry.setText("SPUSTIT ZNOVU");
        retry.setOnClickListener(v -> {
            if (gameView != null) {
                gameView.loadUrl("about:blank");
                gameView.destroy();
                gameView = null;
            }
            launchGameSafely();
        });
        screen.addView(retry);
        setContentView(screen);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) applyImmersiveModeWhenReady();
    }

    @Override
    protected void onDestroy() {
        if (gameView != null) {
            gameView.loadUrl("about:blank");
            gameView.destroy();
            gameView = null;
        }
        super.onDestroy();
    }

    private void applyImmersiveModeWhenReady() {
        Window window = getWindow();
        if (window == null) return;
        View decor = window.getDecorView();
        if (decor == null) {
            if (!immersiveModePending) {
                immersiveModePending = true;
                decor = findViewById(android.R.id.content);
                if (decor != null) decor.post(() -> {
                    immersiveModePending = false;
                    applyImmersiveModeWhenReady();
                });
            }
            return;
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            window.setDecorFitsSystemWindows(false);
            WindowInsetsController controller = decor.getWindowInsetsController();
            if (controller != null) {
                controller.hide(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
                controller.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
            }
        } else {
            getWindow().getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                            | View.SYSTEM_UI_FLAG_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
        }
    }
}
