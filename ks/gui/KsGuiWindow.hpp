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

#ifndef KS_GUI_WINDOW_HPP
#define KS_GUI_WINDOW_HPP

#include <ks/KsSignal.hpp>
#include <ks/shared/KsDeferredProperty.hpp>
#include <ks/shared/KsCallbackTimer.hpp>
#include <ks/gl/KsGLConfig.hpp>
#include <ks/gui/KsGuiConfig.hpp>

namespace ks
{
    namespace gui
    {
        // ============================================================= //

        class Layer
        {
            friend class Window;

        private:
            // Restrict creation to from within Window
            Layer(sint index);

        public:
            ~Layer() = default;

            Layer(Layer const &) = delete;
            Layer(Layer &&) = delete;
            Layer& operator = (Layer const &) = delete;
            Layer& operator = (Layer&&) = delete;

            sint const index;

            // It is expected that a direct connection is used
            // when connecting to these signals
            Signal<> signal_sync;
            Signal<> signal_render;
            Signal<> signal_reset; // graphics context reset
        };

        // ============================================================= //

        class WindowContextThreadInvalid : public ks::Exception
        {
        public:
            WindowContextThreadInvalid(std::string msg);
            ~WindowContextThreadInvalid() = default;
        };

        class WindowLayerIndexInvalid : public ks::Exception
        {
        public:
            WindowLayerIndexInvalid(std::string msg);
            ~WindowLayerIndexInvalid() = default;
        };

        class WindowEventLoopInactive : public ks::Exception
        {
        public:
            WindowEventLoopInactive(std::string msg);
            ~WindowEventLoopInactive() = default;
        };

        // ============================================================= //

        class Window final : public ks::Object
        {
            friend class Application;

        public:
            using base_type = ks::Object;

            using Size = std::pair<uint,uint>;
            using Position = std::pair<sint,sint>;


            // ============================================================= //

            enum class FullscreenMode
            {
                None,
                Desktop,
                Display
            };

            struct Attributes final
            {
                enum class OpenGLAPI {
                    OpenGL,
                    OpenGLES
                };

                enum class OpenGLProfile {
                    Core,
                    Compatibility,
                    Auto
                };

                Attributes() :
                    resizable(true),
                    decorated(true),
                    red_bits(8),
                    green_bits(8),
                    blue_bits(8),
                    alpha_bits(8),
                    depth_bits(24),
                    stencil_bits(8),
                    samples(0),
                    api(OpenGLAPI::OpenGL),
                    profile(OpenGLProfile::Compatibility),
                    version_major(2),
                    version_minor(1),
                    forward_compat(false)
                {
                    // adjust defaults on platform
                    #ifdef KS_ENV_SINGLE_WINDOW
                        resizable = false;
                        decorated = false;
                    #endif

                    #ifdef KS_ENV_GL_ES
                        api = OpenGLAPI::OpenGLES;
                        profile = OpenGLProfile::Auto;
                        version_major = 2;
                        version_minor = 0;
                        forward_compat = false;
                    #endif
                }

                // window attributes
                bool resizable;
                bool decorated;

                // surface attributes
                uint red_bits;
                uint green_bits;
                uint blue_bits;
                uint alpha_bits;
                uint depth_bits;
                uint stencil_bits;
                uint samples;

                // opengl context attributes
                OpenGLAPI api;
                OpenGLProfile profile;
                uint version_major;
                uint version_minor;

                // * this flag removes all functions marked as
                //   'deprecated' in any OpenGL 3.0 or above
                bool forward_compat;
            };

            struct Properties final
            {
                Properties() :
                    width(600),
                    height(360),
                    x(0),
                    y(0),
                    fullscreen(FullscreenMode::None),
                    focused(false),
                    visible(true),
                    always_on_top(false),
                    render_dt_ms(milliseconds(17)),
                    swap_interval(1)
                {
                    // adjust defaults based on platform
                    #ifdef KS_ENV_SINGLE_WINDOW
                        fullscreen = FullscreenMode::Display;
                    #endif
                }

                uint width;
                uint height;
                sint x;
                sint y;
                FullscreenMode fullscreen;
                bool focused;
                bool visible;
                bool always_on_top;
                milliseconds render_dt_ms;
                uint swap_interval;
                std::string title;
            };

            /// * Creates a Window object *after* the corresponding
            ///   system window has been created by ks::gui::Application
            /// * Only ks::gui::Application should ever create objects
            ///   of this class
            /// * Users must call ks::gui::Application::CreateWindow
            ///   to create a window
            /// \param event_loop
            ///     The event loop that will handle this window's
            ///     signals and slots. Note that OpenGL rendering
            ///     will occur in the thread that event_loop is
            ///     running in.
            /// \param context
            ///     The application-generated context for this window
            /// \param req_attribs
            ///     The attributes for the window.
            /// \param req_props
            ///     The initial property values for the window.
            Window(ks::Object::Key const &key,
                   shared_ptr<EventLoop> event_loop,
                   Attributes const &attributes,
                   Properties const &properties);

            void Init(ks::Object::Key const &,
                      shared_ptr<Window> const &this_win);

            ~Window();


            // Unless otherwise stated, none of the methods or
            // properties in this class are thread safe.

            // All interaction with this class must be done
            // from within the same thread as its event loop.

            Attributes const & GetAttributes() const;

            // * Create a layer for the given index
            // * Layers are rendered in order of their indices
            // * Throws: WindowLayerIndexInvalid
            shared_ptr<Layer> CreateLayer(sint index);

            // * Sync a layer (calls Layer::signal_sync::Emit()
            //   with an active context)
            void SyncLayer(shared_ptr<Layer> const &layer);

            // * Stops rendering and signals the Application
            //   to close the window.
            void Close();


            // Properties
            DeferredProperty<Size> size;
            DeferredProperty<Position> position;
            DeferredProperty<FullscreenMode> fullscreen;
            DeferredProperty<bool> focused;
            DeferredProperty<bool> visible;
            DeferredProperty<bool> always_on_top;
            DeferredProperty<milliseconds> render_dt_ms;
            DeferredProperty<uint> swap_interval;
            DeferredProperty<std::string> title;

        private:
            // Window ---> Application
            Signal<> signal_make_context_current;
            Signal<> signal_release_context;
            Signal<> signal_swap_buffers;
            Signal<Id> signal_app_close_window;


            // Application ---> Window
            void onAppInit();
            void onAppPause();
            void onAppResume();
            void onAppQuit();
            void onAppGraphicsReset();
            void onWindowReady();

            // Window ---> Window
            void onRenderDtChanged(milliseconds render_dt_ms);


            // RenderTimer ---> Window
            void onRender();
            void setContextCurrent();


            Attributes m_attributes;
            shared_ptr<CallbackTimer> m_render_timer;
            std::map<sint,shared_ptr<Layer>> m_lkup_layer;
            std::atomic<bool> m_closed;
            std::atomic<bool> m_block_rendering;
        };

    } // gui

} // ks

#endif // KS_GUI_WINDOW_HPP
