// Copyright (c) 2005-2021 Jay Berkenbilt
//
// This file is part of qpdf.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Versions of qpdf prior to version 7 were released under the terms
// of version 2.0 of the Artistic License. At your option, you may
// continue to consider qpdf to be licensed under those terms. Please
// see the manual for additional information.

#ifndef QPDFNAMETREEOBJECTHELPER_HH
#define QPDFNAMETREEOBJECTHELPER_HH

#include <qpdf/QPDFObjectHelper.hh>
#include <qpdf/QPDFObjGen.hh>
#include <map>
#include <memory>

#include <qpdf/DLL.h>

// This is an object helper for name trees. See section 7.9.6 in the
// PDF spec (ISO 32000) for a description of name trees. When looking
// up items in the name tree, use UTF-8 strings. All names are
// normalized for lookup purposes.

class NNTreeImpl;

class QPDFNameTreeObjectHelper: public QPDFObjectHelper
{
  public:
    QPDF_DLL
    QPDFNameTreeObjectHelper(QPDFObjectHandle);
    QPDF_DLL
    virtual ~QPDFNameTreeObjectHelper();

    // Return whether the number tree has an explicit entry for this
    // number.
    QPDF_DLL
    bool hasName(std::string const& utf8);

    // Find an object by name. If found, returns true and initializes
    // oh.
    QPDF_DLL
    bool findObject(std::string const& utf8, QPDFObjectHandle& oh);

    // Return the contents of the name tree as a map. Note that name
    // trees may be very large, so this may use a lot of RAM. It is
    // more efficient to use QPDFNameTreeObjectHelper's iterator.
    QPDF_DLL
    std::map<std::string, QPDFObjectHandle> getAsMap() const;

  private:
    class Members
    {
        friend class QPDFNameTreeObjectHelper;

      public:
        QPDF_DLL
        ~Members();

      private:
        Members(QPDFObjectHandle& oh);
        Members(Members const&) = delete;

        std::shared_ptr<NNTreeImpl> impl;
    };

    PointerHolder<Members> m;
};

#endif // QPDFNAMETREEOBJECTHELPER_HH
