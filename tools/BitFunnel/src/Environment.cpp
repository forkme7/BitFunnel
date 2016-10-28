// The MIT License (MIT)

// Copyright (c) 2016, Microsoft

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "BitFunnel/Index/Factories.h"
#include "BitFunnel/Index/IRecycler.h"
#include "AnalyzeCommand.h"
#include "CdCommand.h"
#include "Environment.h"
#include "ExitCommand.h"
#include "FailOnExceptionCommand.h"
#include "HelpCommand.h"
#include "IngestCommands.h"
#include "QueryCommand.h"
#include "ScriptCommand.h"
#include "ShowCommand.h"
#include "StatusCommand.h"
#include "TaskBase.h"          // TaskBase base class.
#include "TaskFactory.h"
#include "TaskPool.h"
#include "VerifyCommand.h"


namespace BitFunnel
{
    Environment::Environment(IFileSystem& fileSystem,
                             char const * directory,
                             size_t gramSize,
                             size_t threadCount)
        // TODO: Don't like passing *this to TaskFactory.
        // What if TaskFactory calls back before Environment is fully initialized?
        : m_fileSystem(fileSystem),
          m_taskFactory(new TaskFactory(*this)),
          // Start one extra thread for the Recycler.
          m_taskPool(new TaskPool(threadCount + 1)),
          m_index(Factories::CreateSimpleIndex(fileSystem)),
          m_failOnException(false)
    {
        m_index->ConfigureForServing(directory, gramSize, false);
        RegisterCommands();
    }


    void Environment::RegisterCommands()
    {
        m_taskFactory->RegisterCommand<Analyze>();
        m_taskFactory->RegisterCommand<Cd>();
        m_taskFactory->RegisterCommand<Exit>();
        m_taskFactory->RegisterCommand<FailOnException>();
        m_taskFactory->RegisterCommand<Help>();
        m_taskFactory->RegisterCommand<Cache>();
        m_taskFactory->RegisterCommand<Load>();
        m_taskFactory->RegisterCommand<Query>();
        m_taskFactory->RegisterCommand<Script>();
        m_taskFactory->RegisterCommand<Show>();
        m_taskFactory->RegisterCommand<Status>();
        m_taskFactory->RegisterCommand<Verify>();
    }


    void Environment::StartIndex()
    {
        m_index->StartIndex();
    }


    IFileSystem & Environment::GetFileSystem() const
    {
        return m_fileSystem;
    }


    bool Environment::GetFailOnException() const
    {
        return m_failOnException;
    }


    void Environment::SetFailOnException(bool mode)
    {
        m_failOnException = mode;
    }


    std::string const & Environment::GetOutputDir() const
    {
        return m_outputDir;
    }


    void Environment::SetOutputDir(std::string dir)
    {
        m_outputDir = dir;
    }


    TaskFactory & Environment::GetTaskFactory() const
    {
        return *m_taskFactory;
    }


    TaskPool & Environment::GetTaskPool() const
    {
        return *m_taskPool;
    }


    IConfiguration const & Environment::GetConfiguration() const
    {
        return m_index->GetConfiguration();
    }


    IIngestor & Environment::GetIngestor() const
    {
        return m_index->GetIngestor();
    }


    ISimpleIndex const & Environment::GetSimpleIndex() const
    {
        return *m_index;
    }


    ITermTable const & Environment::GetTermTable() const
    {
        return m_index->GetTermTable();
    }
}
