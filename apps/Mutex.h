/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#ifndef _MUTEX_DEFINE
#define _MUTEX_DEFINE

namespace BENCHMARKS {


    class Mutex {

    public:

        Mutex(WSDS::Scheduler* sched);

        void acquire();

        void release();


    private:


        //atomic integer

    }


}

#endif
