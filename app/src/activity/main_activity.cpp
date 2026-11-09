#include "activity/main_activity.hpp"
#include "utils/config.hpp"
#include "api/http.hpp"
#include "view/mpv_core.hpp"

MainActivity::MainActivity() {
    brls::Logger::debug("MainActivity: create");

    auto& conf = AppConfig::instance();
    conf.checkDanmuku();

    std::string query = HTTP::encode_form({
        {"api_key", conf.getToken()},
        {"deviceId", conf.getDeviceId()},
    });

    std::string url = fmt::format("{}/socket?{}", "ws" + conf.getUrl().substr(4), query);
    this->ws = std::make_unique<websocket>(url);
}

void MainActivity::onContentAvailable() {
    auto* button = this->getView("main/fullscreen");
#if (defined(__APPLE__) || defined(__linux__) || defined(_WIN32)) && !defined(ANDROID)
    button->registerClickAction([](brls::View*) {
        VideoContext::FULLSCREEN = !VideoContext::FULLSCREEN;
        AppConfig::instance().setItem(AppConfig::FULLSCREEN, VideoContext::FULLSCREEN);
        brls::Application::getPlatform()->getVideoContext()->fullScreen(VideoContext::FULLSCREEN);
        return true;
    });
    button->addGestureRecognizer(new brls::TapGestureRecognizer(button));
#else
    button->setVisibility(brls::Visibility::GONE);
#endif
}
