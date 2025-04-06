#pragma once

#include <iostream>

#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>

class TestLoggerListener final : public Catch::EventListenerBase
{
public:
    using Catch::EventListenerBase::EventListenerBase;

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override
    {
        std::cout << "\n\n==========================================================================================================";
        std::cout << "\n>>> Running test case: " << testInfo.name << " <<<\n";
        std::cout << "==========================================================================================================\n\n";
    }

    void testCaseEnded(Catch::TestCaseStats const& testCaseStats) override
    {
        std::cout << "\n==========================================================================================================";
        std::cout << "\n<<< Finished test case: " << testCaseStats.testInfo->name << " >>>\n";
        std::cout << "==========================================================================================================\n\n";
    }
};
