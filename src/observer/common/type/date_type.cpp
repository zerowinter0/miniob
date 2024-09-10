/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "common/lang/comparator.h"
#include "common/log/log.h"
#include "common/type/date_type.h"
#include "common/value.h"

int DateType::compare(const Value &left, const Value &right) const
{
  
  ASSERT((left.attr_type() == AttrType::DATES||left.attr_type() == AttrType::CHARS) && (right.attr_type() == AttrType::DATES||right.attr_type() == AttrType::CHARS), "invalid type");
    char* l_date=date_fix::padZeroForSingleDigit(left.value_.pointer_value_);
    char* r_date=date_fix::padZeroForSingleDigit(right.value_.pointer_value_);
    
  int res=common::compare_string(l_date,10,r_date,10);
  delete[] l_date;
  delete[] r_date;
  return res;
}

RC DateType::set_value_from_str(Value &val, const string &data) const
{
  std::string new_date=data;
  if(!date_fix::check_legal_date(&new_date[0])){//必须完全符合date格式
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }
  val.set_date(new_date.c_str());
  return RC::SUCCESS;
}

RC DateType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
    // case AttrType::CHARS:{
    //     result.set_string(val.data(),10);
    //   }break;
    default: return RC::UNIMPLEMENTED;
  }
  return RC::SUCCESS;
}

int DateType::cast_cost(AttrType type)
{
  if (type == AttrType::DATES) {
    return 0;
  }
  return INT32_MAX;
}

RC DateType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << val.value_.pointer_value_;
  result = ss.str();
  return RC::SUCCESS;
}