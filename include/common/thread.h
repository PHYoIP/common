/*
author          Oliver Blaser
date            07.02.2026
copyright       GPL-3.0 - Copyright (c) 2026 Oliver Blaser
*/

#ifndef IG_COMMON_THREAD_H
#define IG_COMMON_THREAD_H

#include <mutex>


namespace thread {

class SharedData
{
public:
    using lock_guard = std::lock_guard<std::mutex>;

public:
    SharedData() {}
    virtual ~SharedData() {}

protected:
    mutable std::mutex m_mtx;

private:
    SharedData(const SharedData& other) = delete;
    SharedData(const SharedData&& other) = delete;
    SharedData& operator=(SharedData& other) = delete;
};

enum class Status
{
    null = 0,
    booting,
    running,
    terminating,
    killed,
};

class ThreadCtl : public SharedData
{
public:
    ThreadCtl()
        : m_status(Status::null), m_sigterm(false), m_sigkill(false)
    {}

    virtual ~ThreadCtl() {}


    // clang-format off
    Status status() const { lock_guard lg(m_mtxThreadCtl); return m_status; }
    void terminate() { lock_guard lg(m_mtxThreadCtl); m_sigterm = true; }
    void kill() { lock_guard lg(m_mtxThreadCtl); m_sigkill = true; }
    // clang-format on


private:
    Status m_status;
    bool m_sigterm;
    bool m_sigkill;
    mutable std::mutex m_mtxThreadCtl;

public:
    // thread internal

    // clang-format off
    void setStatus(const Status& status) { lock_guard lg(m_mtxThreadCtl); m_status = status; } ///< thread internal
    bool sigterm() const { lock_guard lg(m_mtxThreadCtl); return m_sigterm; }                  ///< thread internal
    bool sigkill() const { lock_guard lg(m_mtxThreadCtl); return m_sigkill; }                  ///< thread internal
    // clang-format on
};

} // namespace thread


#endif // IG_COMMON_THREAD_H
