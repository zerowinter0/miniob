/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by WangYunlai on 2022/6/27.
//

#include "common/log/log.h"
#include "sql/operator/update_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"
#include "sql/stmt/update_stmt.h"

RC UpdatePhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  std::unique_ptr<PhysicalOperator> &child = children_[0];
  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  trx_ = trx;

  return RC::SUCCESS;
}

RC UpdatePhysicalOperator::next()
{
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    return RC::RECORD_EOF;
  }

  PhysicalOperator *child = children_[0].get();
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    Record &record = row_tuple->record();

    // // 更新记录字段的值
    // rc = update_record(record);
    // if (rc != RC::SUCCESS) {
    //   LOG_WARN("failed to update record: %s", strrc(rc));
    //   return rc;
    // }

    // 调用事务的更新记录函数
    rc = trx_->update_record(table_, record);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to update record in transaction: %s", strrc(rc));
      return rc;
    }
  }

  return RC::RECORD_EOF;
}

RC UpdatePhysicalOperator::close()
{
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}

// RC UpdatePhysicalOperator::update_record(Record &record)
// {
//   // 遍历需要更新的列，并更新对应的值
//   for (const auto &update_item : update_items_) {
//     const FieldMeta *field_meta = table_->table_meta().field(update_item.column_name);
//     if (nullptr == field_meta) {
//       LOG_WARN("invalid column name: %s", update_item.column_name.c_str());
//       return RC::SCHEMA_FIELD_MISSING;
//     }

//     // 根据字段类型更新值（这里只是简单示例，实际可能涉及类型检查、转换等）
//     memcpy(record.data() + field_meta->offset(), update_item.new_value.c_str(), field_meta->len());
//   }
//   return RC::SUCCESS;
// }