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

#include <algorithm>

#include "BitFunnel/Term.h"
#include "TreatmentPrivateSharedRank0.h"


namespace BitFunnel
{
    //*************************************************************************
    //
    // TreatmentPrivateSharedRank0
    //
    // Terms get one or more rank 0 rows that could be private or shared,
    // depending on term frequency.
    //
    //*************************************************************************
    TreatmentPrivateSharedRank0::TreatmentPrivateSharedRank0(double density,
                                                             double snr)
    {
        // Fill up vector of RowConfigurations. GetTreatment() will use the
        // IdfSum() value of the Term as an index into this vector.
        for (Term::IdfX10 idf = 0; idf <= Term::c_maxIdfX10Value; ++idf)
        {
            RowConfiguration configuration;

            double frequency = Term::IdfX10ToFrequency(idf);
            if (frequency >= density)
            {
                // This term is so common that it must be assigned a private row.
                configuration.push_front(RowConfiguration::Entry(0, 1));
            }
            else
            {
                int k = Term::ComputeRowCount(frequency, density, snr);
                configuration.push_front(RowConfiguration::Entry(0, k));
            }

            m_configurations.push_back(configuration);

            //std::cout << idf / 10.0 << ": ";
            //m_configurations.back().Write(std::cout);
            //std::cout << std::endl;
        }
    }


    RowConfiguration TreatmentPrivateSharedRank0::GetTreatment(Term term) const
    {
        // DESIGN NOTE: we can't c_maxIdfX10Value directly to min because min
        // takes a reference and the compiler has already turned it into a
        // constant, which we can't take a reference to.
        auto local = Term::c_maxIdfX10Value;
        Term::IdfX10 idf = std::min(term.GetIdfSum(), local);
        return m_configurations[idf];
    }
}
