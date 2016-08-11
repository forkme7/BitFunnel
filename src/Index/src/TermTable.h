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

#pragma once

#include <unordered_map>            // std::unordered_map member.
#include <array>                    // std::array member.
#include <vector>                   // std::vector member.

#include "BitFunnel/ITermTable2.h"   // Base class.
//#include "ITermTreatment.h"         // RowConfiguration::Entry::c_maxRowCount.
#include "BitFunnel/RowId.h"        // RowId template parameter.
#include "BitFunnel/Term.h"         // Term::Hash parameter.


namespace BitFunnel
{
    class TermTable : public ITermTable2
    {
    public:
        TermTable();

        // Constructs a TermTable from data previously serialized via the
        // Write() method.
        TermTable(std::istream& input);

        virtual void Write(std::ostream& output) const override;

        virtual void OpenTerm() override;

        virtual void AddRowId(RowId id) override;

        virtual void CloseTerm(Term::Hash hash) override;

        virtual void CloseAdhocTerm(Term::IdfX10 idf,
                                    Term::GramSize gramSize) override;

        virtual PackedRowIdSequence GetRows(const Term& term) const override;

        virtual RowId GetRowIdExplicit(size_t index) const override;

        virtual RowId GetRowIdAdhoc(Term::Hash hash,
                                    size_t index,
                                    size_t variant) const override;


        virtual void SetRowCounts(Rank rank,
                                  size_t explicitCount,
                                  size_t adhocCount) override;

        virtual void Seal() override;

        virtual size_t GetTotalRowCount(Rank rank) const override;

        virtual double GetBytesPerDocument(Rank rank) const override;

    private:
        void ThrowIfSealed() const;

        bool m_setRowCountsCalled;
        bool m_sealed;

        RowIndex m_start;

        // TODO: Is the term table big enough that we would benefit from
        // a more compact class than std::pair<RowIndex, RowIndex>>? Would this
        // even lead to a benefit if we didn't replace std::unordered_map with
        // a better hash table? Should measure actual memory use for this data
        // structure.
        std::unordered_map<Term::Hash, PackedRowIdSequence> m_termHashToRows;

        typedef 
            std::array<
                std::array<PackedRowIdSequence,
                           Term::c_maxGramSize + 1>,
                Term::c_maxIdfX10Value + 1>
            AdhocRecipes;

        // Require AdhocRecipes to be trivailly copyable to allow for binary
        // serialization.
        static_assert(std::is_trivially_copyable<AdhocRecipes>::value,
                      "TermTable: AdhocRecipes must be trivially copyable.");

        AdhocRecipes m_adhocRows;

        std::vector<RowId> m_rowIds;

        std::vector<RowIndex> m_explicitRowCounts;
        std::vector<RowIndex> m_adhocRowCounts;
        std::vector<RowIndex> m_sharedRowCounts;
        RowIndex m_factRowCount;
    };
}
