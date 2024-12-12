std::mutex buffer_mutex;
std::condition_variable buffer_cv;
std::queue<cv::Mat> video_buffer;

// Producer
std::thread producer([&]()
                     { while (true) { 
    std::unique_lock<std::mutex> lock (buffer_mutex); 
    video_buffer.push(captureFrame()); 
    buffer_cv.notify_one(); } });

// Consumer
std::thread consumer([&]()
                     { while (true) { 
        std::unique_lock<std::mutex> lock (buffer_mutex); 
        buffer_cv.wait(lock, [&]() { 
            return !video_buffer.empty(); }); 
            processFrame(video_buffer.front()); 
            video_buffer.pop(); } });