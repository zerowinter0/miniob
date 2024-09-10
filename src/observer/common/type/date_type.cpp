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

bool check_legal_date(std::string& date) {
    std::istringstream date_stream(date);
    int year, month, day;
    char dash;
    std::string new_date="";

    if (!(date_stream >> year >> dash) || dash != '-') {
        return false;
    }

    if (!(date_stream >> month >> dash) || dash != '-') {
        return false;
    }

    if (!(date_stream >> day)) {
        return false;
    }

    if (!(date_stream.eof())) {
        return false;
    }

    if (month < 1 || month > 12) {
        return false;
    }

    // 定义每个月的天数数组
    const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    auto is_leap_year = [](int y) -> bool {
        return (y % 400 == 0) || ((y % 4 == 0) && (y % 100 != 0));
    };

    // 调整二月的天数以适应闰年情况
    if (is_leap_year(year) && month == 2) {
        if (day < 1 || day > 29) {
            return false;
        }
    } else {
        if (day < 1 || (month == 2 && day > 28) || (month != 2 && day > days_in_month[month - 1])) {
            return false;
        }
    }
    new_date+=std::to_string(year)+"-";
    if(month<10)new_date+='0';
    new_date+=std::to_string(month)+"-";
    if(day<10)new_date+='0';
    new_date+=std::to_string(day);
    date=new_date;
    return true;
}

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
  if(!check_legal_date(new_date)){
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }
  val.set_date(new_date.c_str());
  return RC::SUCCESS;
}

RC DateType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
    case AttrType::CHARS:{
        result.set_string(val.data(),10);
      }break;
    default: return RC::UNIMPLEMENTED;
  }
  return RC::SUCCESS;
}

int DateType::cast_cost(AttrType type)
{
  if (type == AttrType::DATES||type==AttrType::CHARS) {
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