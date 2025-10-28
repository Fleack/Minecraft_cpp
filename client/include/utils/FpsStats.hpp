#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <vector>

class FpsStats
{
public:
    void pushFrame(float deltaTime)
    {
        ++m_count;
        float fps = 1.0f / deltaTime;
        float delta = fps - m_mean;
        m_mean += delta / m_count;
        float delta2 = fps - m_mean;
        m_m2 += delta * delta2;

        if (++m_percentileCounter >= PERCENTILE_INTERVAL)
        {
            m_percentileCounter = 0;
            updatePercentiles(fps);
        }

        m_last = fps;
    }

    float currentFps() const
    {
        return m_last;
    }
    float averageFps() const
    {
        return m_mean;
    }
    float fpsStdDev() const
    {
        return m_count > 1 ? std::sqrt(m_m2 / (m_count - 1)) : 0.0f;
    }

    float fps99() const
    {
        return m_fps99;
    }

    float fps999() const
    {
        return m_fps999;
    }

    void reset()
    {
        m_count = 0;
        m_mean = 0;
        m_m2 = 0;
        m_last = 0;
        m_fps99 = 0;
        m_fps999 = 0;
        m_samples.fill(0.0f);
        m_index = 0;
        m_percentileCounter = 0;
    }

private:
    void updatePercentiles(float newSample)
    {
        m_samples[m_index] = newSample;
        m_index = (m_index + 1) % m_samples.size();

        std::vector<float> sorted(m_samples.begin(), m_samples.end());
        std::ranges::sort(sorted);

        m_fps99 = sorted[static_cast<int>(sorted.size() * 0.99f)];
        m_fps999 = sorted[static_cast<int>(sorted.size() * 0.999f)];
    }

    int m_count = 0;
    float m_mean = 0.0f;
    float m_m2 = 0.0f;
    float m_last = 0.0f;

    float m_fps99 = 0.0f;
    float m_fps999 = 0.0f;

    static constexpr std::size_t SAMPLE_SIZE = 256;
    std::array<float, SAMPLE_SIZE> m_samples{};
    std::size_t m_index = 0;

    int m_percentileCounter = 0;
    static constexpr int PERCENTILE_INTERVAL = 10;
};
