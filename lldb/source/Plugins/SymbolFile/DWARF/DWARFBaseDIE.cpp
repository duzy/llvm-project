//===-- DWARFBaseDIE.cpp ---------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "DWARFBaseDIE.h"

#include "DWARFUnit.h"
#include "DWARFDebugInfoEntry.h"
#include "SymbolFileDWARF.h"

#include "lldb/Core/Module.h"
#include "lldb/Symbol/ObjectFile.h"

using namespace lldb_private;

DIERef DWARFBaseDIE::GetDIERef() const {
  if (!IsValid())
    return DIERef();

  dw_offset_t cu_offset = m_cu->GetOffset();
  if (m_cu->GetBaseObjOffset() != DW_INVALID_OFFSET)
    cu_offset = m_cu->GetBaseObjOffset();
  return DIERef(m_cu->GetDebugSection(), cu_offset, m_die->GetOffset());
}

dw_tag_t DWARFBaseDIE::Tag() const {
  if (m_die)
    return m_die->Tag();
  else
    return 0;
}

const char *DWARFBaseDIE::GetTagAsCString() const {
  return lldb_private::DW_TAG_value_to_name(Tag());
}

const char *DWARFBaseDIE::GetAttributeValueAsString(const dw_attr_t attr,
                                                const char *fail_value) const {
  if (IsValid())
    return m_die->GetAttributeValueAsString(GetCU(), attr, fail_value);
  else
    return fail_value;
}

uint64_t DWARFBaseDIE::GetAttributeValueAsUnsigned(const dw_attr_t attr,
                                               uint64_t fail_value) const {
  if (IsValid())
    return m_die->GetAttributeValueAsUnsigned(GetCU(), attr, fail_value);
  else
    return fail_value;
}

uint64_t DWARFBaseDIE::GetAttributeValueAsAddress(const dw_attr_t attr,
                                              uint64_t fail_value) const {
  if (IsValid())
    return m_die->GetAttributeValueAsAddress(GetCU(), attr, fail_value);
  else
    return fail_value;
}

lldb::user_id_t DWARFBaseDIE::GetID() const {
  if (IsValid())
    return GetDWARF()->GetUID(*this);
  return LLDB_INVALID_UID;
}

const char *DWARFBaseDIE::GetName() const {
  if (IsValid())
    return m_die->GetName(m_cu);
  else
    return nullptr;
}

lldb::LanguageType DWARFBaseDIE::GetLanguage() const {
  if (IsValid())
    return m_cu->GetLanguageType();
  else
    return lldb::eLanguageTypeUnknown;
}

lldb::ModuleSP DWARFBaseDIE::GetModule() const {
  SymbolFileDWARF *dwarf = GetDWARF();
  if (dwarf)
    return dwarf->GetObjectFile()->GetModule();
  else
    return lldb::ModuleSP();
}

dw_offset_t DWARFBaseDIE::GetOffset() const {
  if (IsValid())
    return m_die->GetOffset();
  else
    return DW_INVALID_OFFSET;
}

SymbolFileDWARF *DWARFBaseDIE::GetDWARF() const {
  if (m_cu)
    return m_cu->GetSymbolFileDWARF();
  else
    return nullptr;
}

lldb_private::TypeSystem *DWARFBaseDIE::GetTypeSystem() const {
  if (m_cu)
    return m_cu->GetTypeSystem();
  else
    return nullptr;
}

DWARFASTParser *DWARFBaseDIE::GetDWARFParser() const {
  lldb_private::TypeSystem *type_system = GetTypeSystem();
  if (type_system)
    return type_system->GetDWARFParser();
  else
    return nullptr;
}

bool DWARFBaseDIE::HasChildren() const {
  return m_die && m_die->HasChildren();
}

bool DWARFBaseDIE::Supports_DW_AT_APPLE_objc_complete_type() const {
  return IsValid() && GetDWARF()->Supports_DW_AT_APPLE_objc_complete_type(m_cu);
}

size_t DWARFBaseDIE::GetAttributes(DWARFAttributes &attributes,
                               uint32_t depth) const {
  if (IsValid())
    return m_die->GetAttributes(m_cu, attributes, depth);
  if (depth == 0)
    attributes.Clear();
  return 0;
}

bool operator==(const DWARFBaseDIE &lhs, const DWARFBaseDIE &rhs) {
  return lhs.GetDIE() == rhs.GetDIE() && lhs.GetCU() == rhs.GetCU();
}

bool operator!=(const DWARFBaseDIE &lhs, const DWARFBaseDIE &rhs) {
  return !(lhs == rhs);
}

const DWARFDataExtractor &DWARFBaseDIE::GetData() const {
  // Clients must check if this DIE is valid before calling this function.
  assert(IsValid());
  return m_cu->GetData();
}
