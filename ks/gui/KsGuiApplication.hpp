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

#ifndef KS_GUI_APPLICATION_HPP
#define KS_GUI_APPLICATION_HPP

#include <ks/KsObject.hpp>
#include <ks/KsSignal.hpp>
#include <ks/gui/KsGuiScreen.hpp>
#include <ks/gui/KsGuiWindow.hpp>
#include <ks/gui/KsGuiInput.hpp>

namespace ks
{
	namespace gui
    {
        class IPlatform;
        class IPlatformWindow;

        class Application : public ks::Object
		{
            // This needs to be up here for order of init
            IPlatform* m_platform;

        public:
            using base_type = ks::Object;

            Application(ks::Object::Key const &key);

            void Init(ks::Object::Key const &key,
                      shared_ptr<Application> const &app);

            ~Application();

            // Unless otherwise mentioned, none of these functions
            // are thread safe and should be called from the main thread

            // * Processes any waiting events
            // * This needs to be invoked manually
            void ProcessEvents();

            // * Starts the application event loop, blocking the
            //   calling thread
            void Run();

            // * Gets a list of the current screens
            std::vector<shared_ptr<Screen const>> GetScreens();

            /// * Creates a Window
            /// * The final Window parameters may differ from the
            ///   requested parameters based on system capabilities
            /// \param win_attrs
            ///     Attributes (generally unchanging parameters) for
            ///     for window creation
            /// \param win_init
            ///     Properties (parameters that may change) for
            ///     window creation
            /// \param event_loop
            ///     The event loop that will handle this window's
            ///     signals and slots. Note that OpenGL rendering
            ///     will occur in the thread that event_loop is
            ///     running in.
            shared_ptr<Window> CreateWindow(shared_ptr<EventLoop> event_loop,
                                            Window::Attributes win_attrs,
                                            Window::Properties win_props);

            // * Tells the application to start quitting
            // * Returns immediately. Calling quit will eventually
            //   stop the main EventLoop and cause Run() to return
            void Quit();

            // Signals

            // Application
            Signal<> signal_init;
            Signal<> signal_pause;
            Signal<> signal_resume;
            Signal<> signal_quit;
            Signal<> signal_low_memory;

            // * emitted when the OpenGL context is lost
            //   and all buffers/texture data should be
            //   recreated
            Signal<> signal_graphics_reset;

            // Display Screens
            // * emitted when screens have been added or removed
            Signal<> signal_screen_config_changed;

            // Windows
            // * emitted when the last open window is closed
            // * the application will return from the main loop
            //   shortly after this signal is sent
            Signal<> signal_last_window_closed;

            // Input
            Signal<KeyEvent>* const signal_keyboard_input;
            Signal<std::string>* const signal_utf8_input;
            Signal<MouseEvent>* const signal_mouse_input;
            Signal<ScrollEvent>* const signal_scroll_input;

            // * emitted after events are processed (ie after
            //   the ProcessEvents call is completed)
            // bool - True if any new events were processed
            Signal<bool>* const signal_processed_events;


        private:
            void onInit();
            void onPause();
            void onResume();
            void onLowMemory();
            void onGraphicsReset();
            void onCloseWindow(Id win_id);
            void onLastWindowClosed();

            bool m_quitting;

            // * We don't hang on to ks::gui::Window shared_ptrs so
            //   that they can be automatically destroyed when the
            //   user's window ref count goes to 0
            struct PlatformWindowDesc
            {
                Id id;
                shared_ptr<IPlatformWindow> platform_window;
                shared_ptr<ks::ConnectionContext> connection_context;
            };

            std::vector<PlatformWindowDesc> m_list_windows;
		};
			
	} // gui

} // ks

#endif // KS_GUI_APPLICATION_HPP
