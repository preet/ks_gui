/*
   Copyright (C) 2015-2016 Preet Desai (preet.desai@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <ks/gui/KsGuiApplication.hpp>
#include <ks/gui/KsGuiPlatform.hpp>

namespace ks
{
    namespace gui
    {       
        namespace {
            shared_ptr<IPlatform> g_platform;

            IPlatform* SetupPlatform(shared_ptr<EventLoop> const &event_loop)
            {
                if(g_platform)
                {
                    // Only one instance of Application and Platform should exist
                    throw PlatformInitFailed(
                                "SDL: Platform instance already exists");
                }

                // Start the event loop immediately so we can
                // handle events and tasks before calling Run
                event_loop->Start();
                g_platform = CreatePlatform(event_loop);

                return g_platform.get();
            }
        }

        Application::Application(ks::Object::Key const &key) :
            ks::Object(key,make_shared<EventLoop>()),

            m_platform(SetupPlatform(GetEventLoop())),

            signal_keyboard_input(
                &(m_platform->signal_keyboard_input)),

            signal_utf8_input(
                &(m_platform->signal_utf8_input)),

            signal_mouse_input(
                &(m_platform->signal_mouse_input)),

            signal_scroll_input(
                &(m_platform->signal_scroll_input)),

            signal_processed_events(
                &(m_platform->signal_processed_events)),

            m_quitting(false)
        {

        }

        void Application::Init(ks::Object::Key const &,
                               shared_ptr<Application> const &this_app)
        {
            // Setup connections

            // Platform ---> Application
            g_platform->signal_pause.Connect(
                        this_app,
                        &Application::onPause,
                        ks::ConnectionType::Direct);

            g_platform->signal_resume.Connect(
                        this_app,
                        &Application::onResume,
                        ks::ConnectionType::Direct);

            g_platform->signal_quit.Connect(
                        this_app,
                        &Application::Quit,
                        ks::ConnectionType::Direct);

            g_platform->signal_low_memory.Connect(
                        this_app,
                        &Application::onLowMemory,
                        ks::ConnectionType::Direct);

            g_platform->signal_graphics_reset.Connect(
                        this_app,
                        &Application::onGraphicsReset,
                        ks::ConnectionType::Direct);

            this_app->signal_last_window_closed.Connect(
                        this_app,
                        &Application::onLastWindowClosed);
        }

        Application::~Application()
        {}

        void Application::ProcessEvents()
        {
            g_platform->ProcessEvents();
        }

        void Application::Run()
        {
            LOG.Trace() << "Application::Run";

            signal_init.Emit();
            g_platform->Run();

            LOG.Trace() << "Application::Run returned";
        }

        std::vector<shared_ptr<Screen const>> Application::GetScreens()
        {
            return g_platform->GetScreens();
        }

        shared_ptr<Window>
        Application::CreateWindow(shared_ptr<EventLoop> window_evl,
                                  Window::Attributes win_attrs,
                                  Window::Properties win_props)
        {
            shared_ptr<Application> this_app =
                    std::static_pointer_cast<Application>(
                        shared_from_this());

            // Create the window
            shared_ptr<IPlatformWindow> platform_window =
                    g_platform->CreateWindow(
                        window_evl,win_attrs,win_props);

            shared_ptr<Window> window =
                    make_object<Window>(
                        window_evl,win_attrs,win_props);

            m_list_windows.emplace_back(
                        PlatformWindowDesc{
                            window->GetId(),
                            platform_window,
                            make_object<ks::ConnectionContext>(
                                this_app->GetEventLoop())
                        });

            PlatformWindowDesc& desc = m_list_windows.back();

            // Setup connections

            // PlatformWindow ---> Window
            platform_window->signal_size_changed.Connect(
                        &window->size,
                        &DeferredProperty<gui::Window::Size>::Notify,
                        window);

            platform_window->signal_position_changed.Connect(
                        &window->position,
                        &DeferredProperty<Window::Position>::Notify,
                        window);

            platform_window->signal_fullscreen_changed.Connect(
                        &window->fullscreen,
                        &DeferredProperty<gui::Window::FullscreenMode>::Notify,
                        window);

            platform_window->signal_focused_changed.Connect(
                        &window->focused,
                        &DeferredProperty<bool>::Notify,
                        window);

            platform_window->signal_visible_changed.Connect(
                        &window->visible,
                        &DeferredProperty<bool>::Notify,
                        window);

            platform_window->signal_always_on_top_changed.Connect(
                        &window->always_on_top,
                        &DeferredProperty<bool>::Notify,
                        window);

            platform_window->signal_swap_interval_changed.Connect(
                        &window->swap_interval,
                        &DeferredProperty<uint>::Notify,
                        window);

            platform_window->signal_title_changed.Connect(
                        &window->title,
                        &DeferredProperty<std::string>::Notify,
                        window);

            platform_window->signal_close.Connect(
                        window,
                        &Window::Close);


            // Window ---> PlatformWindow
            window->size.signal_set.Connect(
                        platform_window.get(),
                        &IPlatformWindow::SetSize,
                        desc.connection_context);

            window->position.signal_set.Connect(
                        platform_window.get(),
                        &IPlatformWindow::SetPosition,
                        desc.connection_context);

            window->fullscreen.signal_set.Connect(
                        platform_window.get(),
                        &IPlatformWindow::SetFullscreen,
                        desc.connection_context);

            // Note the direct connections since we need
            // to invoke these functions from the context
            // of the render thread
            window->signal_make_context_current.Connect(
                        platform_window.get(),
                        &IPlatformWindow::MakeContextCurrent,
                        desc.connection_context,
                        ks::ConnectionType::Direct);

            window->signal_release_context.Connect(
                        platform_window.get(),
                        &IPlatformWindow::ReleaseContext,
                        desc.connection_context,
                        ks::ConnectionType::Direct);

            window->signal_swap_buffers.Connect(
                        platform_window.get(),
                        &IPlatformWindow::SwapBuffers,
                        desc.connection_context,
                        ks::ConnectionType::Direct);


            // Window ---> Application
            window->signal_app_close_window.Connect(
                        this_app,
                        &Application::onCloseWindow);


            // Application ---> Window
            this_app->signal_init.Connect(
                        window,
                        &Window::onAppInit);

            this_app->signal_pause.Connect(
                        window,
                        &Window::onAppPause,
                        ks::ConnectionType::Blocking);

            this_app->signal_resume.Connect(
                        window,
                        &Window::onAppResume,
                        ks::ConnectionType::Blocking);

            this_app->signal_quit.Connect(
                        window,
                        &Window::onAppQuit,
                        ks::ConnectionType::Blocking);

            this_app->signal_graphics_reset.Connect(
                        window,
                        &Window::onAppGraphicsReset);


            // Start rendering
            window_evl->PostTask(
                        make_shared<Task>(
                            [window](){
                                window->onWindowReady();
                            }));

            return window;
        }

        void Application::Quit()
        {
            if(!m_quitting) {
                LOG.Trace() << "Application::Quit";
                m_quitting = true;
                signal_quit.Emit();
                g_platform->Quit();
            }
        }

        void Application::onPause()
        {
            LOG.Trace() << "Application::onPause";
            signal_pause.Emit();
        }

        void Application::onResume()
        {
            LOG.Trace() << "Application::onResume";
            signal_resume.Emit();
        }

        void Application::onLowMemory()
        {
            LOG.Trace() << "Application::onLowMemory";
            signal_low_memory.Emit();
        }

        void Application::onGraphicsReset()
        {
            LOG.Trace() << "Application::onGraphicsReset";
            signal_graphics_reset.Emit();
        }

        void Application::onCloseWindow(Id win_id)
        {
            auto it = std::find_if(
                        m_list_windows.begin(),
                        m_list_windows.end(),
                        [win_id](PlatformWindowDesc const &window_desc) {
                            return (window_desc.id == win_id);
                        }
                    );

            if(it != m_list_windows.end()) {
                g_platform->DestroyWindow(it->platform_window);
                m_list_windows.erase(it);
            }

            LOG.Trace() << "Application::onCloseWindow";
            if(m_list_windows.empty()) {
                LOG.Trace() << "onCloseWindow emit lastWindowClosed";
                signal_last_window_closed.Emit();
            }
        }

        void Application::onLastWindowClosed()
        {
            LOG.Trace() << "Application::onLastWindowClosed";
        }
    }
}
