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

#ifndef INCLUDE_NITRO_LOG_MPI_RANK_ATTRIBUTE_HPP
#define INCLUDE_NITRO_LOG_MPI_RANK_ATTRIBUTE_HPP

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/intercommunicator.hpp>

namespace nitro
{
namespace log
{

    class mpi_rank_attribute
    {
        int my_mpi_rank;

    public:
        mpi_rank_attribute() : my_mpi_rank(0)
        {
            if (boost::mpi::environment::initialized())
            {
                // lets try, if there is a parent communicator
                MPI_Comm pcomm;
                int ret = MPI_Comm_get_parent(&pcomm);
                if (ret == MPI_SUCCESS && pcomm != MPI_COMM_NULL)
                {
                    boost::mpi::intercommunicator comm(pcomm, boost::mpi::comm_attach);

                    my_mpi_rank = comm.rank() + comm.remote_size();
                }
                else
                {
                    boost::mpi::communicator comm;
                    my_mpi_rank = comm.rank();
                }
            }
        }

        int mpi_rank() const
        {
            return my_mpi_rank;
        }
    };
}
} // namespace nitro::log

#endif // INCLUDE_NITRO_LOG_THREAD_ID_ATTRIBUTE_HPP
