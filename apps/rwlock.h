/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#ifndef _RWLOCK_DEFINE
#define _RWLOCK_DEFINE

#include "Mutex.h"

namespace BENCHMARKS {


    class RWLock {

    public:

        RWLock(WSDS::scheduler* sched);

        //reader grab reader writer lock
        void startRead();

        //reader remove lock
        void endRead();


        //writer grabs lock
        void startWrite();


        //writer release lock
        void endWrite();


    private:
        
        Mutex * mutex;
        Mutex * wlock;
        int num_readers;
             

    }


}

#endif
