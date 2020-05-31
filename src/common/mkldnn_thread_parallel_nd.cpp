/*******************************************************************************
* Copyright 2018 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "mkldnn_thread_parallel_nd.hpp"

namespace mkldnn {
namespace impl {

/* general parallelization */
void parallel(int nthr, size_t work_amount, const std::function<void(int, int)> & f) {
    if (nthr == 0) nthr = mkldnn_get_max_threads();
#if MKLDNN_THR == MKLDNN_THR_SEQ
    assert(nthr == 1);
    f(0, 1);
#elif MKLDNN_THR == MKLDNN_THR_OMP
    if (nthr == 1) { f(0, 1); return; }
#   pragma omp parallel num_threads(nthr)
    f(mkldnn_get_thread_num(), mkldnn_get_num_threads());
#elif MKLDNN_THR == MKLDNN_THR_TBB
    if (work_amount < (size_t)nthr) nthr = (int)work_amount;
    int max_nthr = mkldnn_get_max_threads();
    if (nthr == 0 || nthr > max_nthr) nthr = max_nthr;
    if (nthr == 1) { f(0, 1); return; }
    tbb::parallel_for(0, nthr, [&](int ithr) { f(ithr, nthr); }, tbb::static_partitioner());
#elif MKLDNN_THR == MKLDNN_THR_TBB_AUTO
    if (nthr == 1) { f(0, 1); return; }
    tbb::parallel_for(0, nthr, [&](int ithr) { f(ithr, nthr); });
#endif
}

void parallel(int nthr, const std::function<void(int, int)> & f) {
    if (nthr == 0) nthr = mkldnn_get_max_threads();
#if MKLDNN_THR == MKLDNN_THR_SEQ
    assert(nthr == 1);
    f(0, 1);
#elif MKLDNN_THR == MKLDNN_THR_OMP
    if (nthr == 1) { f(0, 1); return; }
#   pragma omp parallel num_threads(nthr)
    f(mkldnn_get_thread_num(), mkldnn_get_num_threads());
#elif MKLDNN_THR == MKLDNN_THR_TBB
    if (nthr == 1) { f(0, 1); return; }
    tbb::parallel_for(0, nthr, [&](int ithr) { f(ithr, nthr); }, tbb::static_partitioner());
#elif MKLDNN_THR == MKLDNN_THR_TBB_AUTO
    if (nthr == 1) { f(0, 1); return; }
    tbb::parallel_for(0, nthr, [&](int ithr) { f(ithr, nthr); });
#endif
}

}  // namespace impl
}  // namespace mkldnn