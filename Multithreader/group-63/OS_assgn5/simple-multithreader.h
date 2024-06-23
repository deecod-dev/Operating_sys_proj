    #ifndef SIMPLE_MULTITHREADER_H
    #define SIMPLE_MULTITHREADER_H

    #include <iostream>
    #include <functional>
    #include <pthread.h>
    #include <chrono>

    // Wrapper class for Pthreads to simplify thread management
    class ThreadWrapper {
    public:
        ThreadWrapper() : lambda([]{}) {}
        ThreadWrapper(std::function<void()> &&lambda) : lambda(lambda) {}

        // Static function to be passed to pthread_create
        static void* startThread(void *obj) {
            ThreadWrapper *wrapper = static_cast<ThreadWrapper*>(obj);
            wrapper->lambda();
            return nullptr;
        }

        void join() {
            int res=pthread_join(thread, nullptr);
            if (res != 0)
            {
                perror("Error in thread join\n");
            }
        }
        void create() {
            int result=pthread_create(&thread, nullptr, &ThreadWrapper::startThread, this);
            if (result != 0)
            {
                perror("Error in thread creation\n");
            }
        }
        pthread_t getThreadId() const {
            return thread;
        }

    private:
        pthread_t thread;
        std::function<void()> lambda;
    };

    // Parallel for implementation for a single-dimensional loop
    void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads) {
        auto start = std::chrono::high_resolution_clock::now();
        if (numThreads < 1) {
            lambda(low);  // Execute the lambda on the main thread
            return;
        }
        int range = high - low + 1;
        int chunkSize = range / numThreads;
        int extra = range % numThreads;

        ThreadWrapper threads[numThreads];

        for (int i = 0; i < numThreads; ++i)
        {
            int threadLow = low + i * chunkSize;
            int threadHigh = threadLow + chunkSize - 1;
            if(threadHigh>=high)
            {
                printf("gret\n");
                threadHigh=high-1;
            }

            threads[i] = ThreadWrapper([lambda, threadLow, threadHigh]()
            {
                for (int j = threadLow; j <= threadHigh; ++j)
                {
                    lambda(j);
                }
            });

            threads[i].create();
        }

        for (int i = 0; i < numThreads; ++i)
        {
            int res=pthread_join(threads[i].getThreadId(),NULL);
            if (res != 0)
            {
                perror("Error in thread join\n");
            }
            // threads[i].join();
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Execution time: " << duration << " milliseconds\n";
        // std::cout << "1d loop\n";
    }

    // Parallel for implementation for a two-dimensional loop
    void parallel_for(int low1, int high1, int low2, int high2,
        std::function<void(int, int)> &&lambda, int numThreads) {
            // std::cout <<"numthreds-" << numThreads;
        if (numThreads < 1) {
            lambda(low1, low2);  // Execute the lambda on the main thread
            return;
        }
        // std::cout << "\n2d loop before auto start\n";

        auto start = std::chrono::high_resolution_clock::now();
        
        int range1 = high1 - low1 + 1;
        int range2 = high2 - low2 + 1;

        ThreadWrapper threads[numThreads];

        for (int i = 0; i < numThreads; ++i) {
            int threadLow1 = low1 + (i * range1) / numThreads;
            int threadHigh1 = low1 + ((i + 1) * range1) / numThreads - 1;
            if(threadHigh1>=high1)
            {
                // printf("gret\n");
                threadHigh1=high1-1;
            }
            threads[i] = ThreadWrapper([lambda, threadLow1, threadHigh1, low2, high2]() {
                for (int j = threadLow1; j <= threadHigh1; ++j) {
                    for (int k = low2; k <= high2; ++k) {
                        lambda(j, k);
                    }
                }
            });

            threads[i].create();
        }
        // std::cout <<"numthreds-" << numThreads;

        // std::cout << "\n2d loop before chota for loop\n";
        // printf("hhhhhhhhhhh\n");
        // printf("%daaa",numThreads);
        for (int i = 0; i < numThreads; ++i) {
            // printf("kyaaaa\n");
            // std::cout <<"numthreds-" << numThreads;
            // std::cout << "2d loop chota for loop i-"<<i;
            threads[i].join();
        }
        // printf("here\n");
        // std::cout << "2d loop before auto end\n";

        auto end = std::chrono::high_resolution_clock::now();
        // std::cout << "2d loop before auto duration\n";
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Execution time: " << duration << " milliseconds\n";
        // std::cout << "2d loop\n";
    }

    #endif // SIMPLE_MULTITHREADER_H
