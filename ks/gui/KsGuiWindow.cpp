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

#include <ks/gui/KsGuiWindow.hpp>
#include <ks/gui/KsGuiApplication.hpp>
#include <ks/KsTimer.hpp>

namespace ks
{
    namespace gui
    {
        // ============================================================= //

        Layer::Layer(sint index) :
            index(index)
        {}

        // ============================================================= //

        WindowContextThreadInvalid::WindowContextThreadInvalid(std::string msg) :
            ks::Exception(ks::Exception::ErrorLevel::FATAL,std::move(msg),true)
        {}

        WindowLayerIndexInvalid::WindowLayerIndexInvalid(std::string msg) :
            ks::Exception(ks::Exception::ErrorLevel::WARN,std::move(msg),true)
        {}

        WindowEventLoopInactive::WindowEventLoopInactive(std::string msg) :
            ks::Exception(ks::Exception::ErrorLevel::FATAL,std::move(msg),true)
        {}

        // ============================================================= //

        Window::Window(ks::Object::Key const &key,
                       shared_ptr<EventLoop> event_loop,
                       Attributes const &attributes,
                       Properties const &p) :
            ks::Object(key,event_loop),
            size(Size(p.width,p.height)),
            position(Position(p.x,p.y)),
            fullscreen(p.fullscreen),
            focused(p.focused),
            visible(p.visible),
            always_on_top(p.always_on_top),
            swap_interval(p.swap_interval),
            title(p.title),
            m_attributes(attributes),
            m_closed(false),
            m_block_rendering(true)
        {}

        void Window::Init(ks::Object::Key const &,
                          shared_ptr<Window> const &this_win)
        {

        }

        Window::~Window()
        {
            // The EventLoop for this Window must be alive when
            // this destructor is called *or* the Window must
            // already have been closed.

            // Calling the destructor with this Window still
            // open and a stopped EventLoop will deadlock.

            // Example:
            // In the common case where a Window is created in
            // the same scope and with the same EventLoop as the
            // Application, the Application will close all Windows
            // before it returns from Run() because otherwise the
            // EventLoop would be stopped with an open window when
            // this destructor is called:

            // 1. Application created
            // 2. Window created
            // 3. Application->Run()
            // 4. Run returns() // Application EventLoop stopped
            // 5. Window destroyed

            // In this specific case, the Window will have already
            // been closed by the Application and the destructor
            // doesn't have to do anything.

            // In all other cases, the developer must ensure
            // the conditions outlined above or this destructor
            // will throw a fatal error.

            if(!m_closed)
            {
                if(!this->GetEventLoop()->GetRunning()) {
                    throw WindowEventLoopInactive(
                                "Window: Destructor called with "
                                "inactive EventLoop");
                }

                // Post a close task on this Window's EventLoop
                auto close_task =
                        make_shared<Task>(
                            [this](){
                                this->Close();
                            });

                this->GetEventLoop()->PostTask(close_task);
                close_task->Wait();
            }
        }

        shared_ptr<Layer> Window::CreateLayer(sint index)
        {
            auto it = m_lkup_layer.find(index);

            if(it == m_lkup_layer.end()) {
                shared_ptr<Layer> layer(new Layer(index));
                it = m_lkup_layer.emplace(index,layer).first;

                return layer;
            }

            throw WindowLayerIndexInvalid(
                        "Window: Layer index already exists");
        }

        void Window::SyncLayer(shared_ptr<Layer> const &layer)
        {
            if(m_block_rendering) {
                return;
            }

            setContextCurrent();
            layer->signal_sync.Emit();
        }

        void Window::Render()
        {
            if(m_block_rendering) {
                return;
            }

            setContextCurrent();

            for(auto& idx_layer : m_lkup_layer) {
                idx_layer.second->signal_render.Emit();
            }

            signal_swap_buffers.Emit();
        }

        void Window::Close()
        {
            if(!m_closed) {
                m_block_rendering = true;

                signal_release_context.Emit();
                signal_app_close_window.Emit(this->GetId());
                m_closed = true;
            }

            LOG.Trace() << "Window::Close";
        }

        void Window::onAppInit()
        {
            m_block_rendering = false;
        }

        void Window::onAppPause()
        {
            m_block_rendering = true;

            // Required on Android/SDL to recreate the EGL surface
            signal_release_context.Emit();
        }

        void Window::onAppResume()
        {
            m_block_rendering = false;
        }

        void Window::onAppQuit()
        {
            this->Close();
        }

        void Window::onAppGraphicsReset()
        {
            // Nothing needs to be done
        }

        void Window::onWindowReady()
        {
            m_block_rendering = false;
        }

        void Window::setContextCurrent()
        {
            signal_make_context_current.Emit();
        }

        // ============================================================= //

    } // gui
} // ks
