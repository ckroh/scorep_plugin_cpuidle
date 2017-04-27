/*
 * Copyright (c) 2015-2016, Technische Universität Dresden, Germany
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 *    endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef INCLUDE_SCOREP_PLUGIN_POLICY_SYNCHRONIZE_HPP
#define INCLUDE_SCOREP_PLUGIN_POLICY_SYNCHRONIZE_HPP

#include <scorep/plugin/interface.hpp>

#include <cstdint>
#include <limits>

namespace scorep
{
namespace plugin
{
    namespace policy
    {
        namespace detail
        {
            template <typename Plugin, typename Policies>
            class synchronize_dummy
            {
                template <bool Test = false>
                void synchronize(bool, SCOREP_MetricSynchronizationMode)
                {
                    static_assert(Test,
                                  "The synchronize policy requires the definition of the method"
                                  "`void synchronize(bool, SCOREP_MetricSynchronizationMode)`");
                }
            };
        }

        template <typename Plugin, typename Policies>
        class synchronize : private detail::synchronize_dummy<Plugin, Policies>
        {
        public:
            static void build_info(SCOREP_Metric_Plugin_Info& info)
            {
                info.synchronize = synchronize_handler;
            }

            static void synchronize_handler(bool is_responsible,
                                            SCOREP_MetricSynchronizationMode sync_mode)
            {
                try
                {
                    Plugin::instance().synchronize(is_responsible, sync_mode);
                }
                catch (std::exception& e)
                {
                    Plugin::instance().print_uncaught_exception(e);
                }
            }
        };
    }
}
}

#endif // INCLUDE_SCOREP_PLUGIN_POLICY_SYNCHRONIZE_HPP
