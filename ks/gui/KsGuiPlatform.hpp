/*
   Copyright (C) 2015 Preet Desai (preet.desai@gmail.com)

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

#ifndef KS_GUI_PLATFORM_HPP
#define KS_GUI_PLATFORM_HPP

#include <ks/gui/KsGuiWindow.hpp>
#include <ks/gui/KsGuiScreen.hpp>

namespace ks
{
    namespace gui
    {
        // ============================================================= //

        class PlatformInitFailed : public ks::Exception
        {
        public:
            PlatformInitFailed(std::string msg);
            ~PlatformInitFailed() = default;
        };

        class WindowCreationFailed : public ks::Exception
        {
        public:
            WindowCreationFailed(std::string msg);
            ~WindowCreationFailed() = default;
        };

        // should be a warning
        class WindowSettingFailed : public ks::Exception
        {
        public:
            WindowSettingFailed(std::string msg);
            ~WindowSettingFailed() = default;
        };

        class WindowContextMakeCurrentError : public ks::Exception
        {
        public:
            WindowContextMakeCurrentError(std::string msg);
            ~WindowContextMakeCurrentError() = default;
        };


        // ============================================================= //

        class IPlatformWindow
        {
        public:
            virtual ~IPlatformWindow() = default;

            // Application ---> PlatformWindow
            virtual bool IsCurrentContext() = 0;
            virtual void MakeContextCurrent() = 0;
            virtual void ReleaseContext() = 0;
            virtual void SwapBuffers() = 0;

            virtual void SetSize(Window::Size const &size) = 0;
            virtual void SetPosition(Window::Position const &position) = 0;
            virtual void SetFullscreen(Window::FullscreenMode fullscreen) = 0;
            virtual void SetFocused(bool focused) = 0;
            virtual void SetVisible(bool visible) = 0;
            virtual void SetAlwaysOnTop(bool always_on_top) = 0;
            virtual void SetSwapInterval(uint swap_interval) = 0;
            virtual void SetTitle(std::string const &title) = 0;
            virtual void Destroy() = 0;

            // PlatformWindow ---> Window
            Signal<Window::Size> signal_size_changed;
            Signal<Window::Position> signal_position_changed;
            Signal<Window::FullscreenMode> signal_fullscreen_changed;
            Signal<bool> signal_focused_changed;
            Signal<bool> signal_visible_changed;
            Signal<bool> signal_always_on_top_changed;
            Signal<uint> signal_swap_interval_changed;
            Signal<std::string> signal_title_changed;
            Signal<> signal_close;
        };

        // ============================================================= //

        class IPlatform
        {
        public:
            IPlatform() = default;
            virtual ~IPlatform() = default;

            //
            virtual void Run() = 0;
            virtual void Quit() = 0;

            // Display Screens
            virtual std::vector<shared_ptr<Screen const>> GetScreens() = 0;

            // Windows
            virtual shared_ptr<IPlatformWindow>
            CreateWindow(Window::Attributes& win_attrs,
                         Window::Properties& win_props) = 0;

            virtual void DestroyWindow(shared_ptr<IPlatformWindow>) = 0;


            Signal<> signal_init;
            Signal<> signal_pause;
            Signal<> signal_resume;
            Signal<> signal_quit;
            Signal<> signal_low_memory;
            Signal<> signal_graphics_reset;
        };

        // ============================================================= //

        extern shared_ptr<IPlatform> CreatePlatform(shared_ptr<EventLoop> app_event_loop);

        // ============================================================= //
    }
}


#endif // KS_GUI_PLATFORM_HPP
