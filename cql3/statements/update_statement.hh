/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Copyright 2015 Cloudius Systems
 *
 * Modified by Cloudius Systems
 */

#ifndef CQL_STATEMENTS_UPDATE_STATEMENT_HH
#define CQL_STATEMENTS_UPDATE_STATEMENT_HH

#include "cql3/statements/modification_statement.hh"
#include "cql3/column_identifier.hh"
#include "cql3/term.hh"

#include <vector>

#if 0
package org.apache.cassandra.cql3.statements;

import java.nio.ByteBuffer;
import java.util.*;

import org.apache.cassandra.cql3.*;
import org.apache.cassandra.config.CFMetaData;
import org.apache.cassandra.config.ColumnDefinition;
import org.apache.cassandra.db.*;
import org.apache.cassandra.db.composites.Composite;
import org.apache.cassandra.db.index.SecondaryIndexManager;
import org.apache.cassandra.exceptions.*;
import org.apache.cassandra.utils.ByteBufferUtil;
import org.apache.cassandra.utils.Pair;
#endif

namespace cql3 {

namespace statements {

/**
 * An <code>UPDATE</code> statement parsed from a CQL query statement.
 *
 */
class update_statement : public modification_statement {
private:
#if 0
    private static final Constants.Value EMPTY = new Constants.Value(ByteBufferUtil.EMPTY_BYTE_BUFFER);
#endif

    update_statement(statement_type type, int32_t bound_terms, schema_ptr s, std::unique_ptr<attributes>&& attrs)
        : modification_statement{type, bound_terms, std::move(s), std::move(attrs)}
    { }

    virtual bool require_full_clustering_key() const {
        return true;
    }

#if 0
    public void addUpdateForKey(ColumnFamily cf, ByteBuffer key, Composite prefix, UpdateParameters params)
    throws InvalidRequestException
    {
        // Inserting the CQL row marker (see #4361)
        // We always need to insert a marker for INSERT, because of the following situation:
        //   CREATE TABLE t ( k int PRIMARY KEY, c text );
        //   INSERT INTO t(k, c) VALUES (1, 1)
        //   DELETE c FROM t WHERE k = 1;
        //   SELECT * FROM t;
        // The last query should return one row (but with c == null). Adding the marker with the insert make sure
        // the semantic is correct (while making sure a 'DELETE FROM t WHERE k = 1' does remove the row entirely)
        //
        // We do not insert the marker for UPDATE however, as this amount to updating the columns in the WHERE
        // clause which is inintuitive (#6782)
        //
        // We never insert markers for Super CF as this would confuse the thrift side.
        if (type == StatementType.INSERT && cfm.isCQL3Table() && !prefix.isStatic())
            cf.addColumn(params.makeColumn(cfm.comparator.rowMarker(prefix), ByteBufferUtil.EMPTY_BYTE_BUFFER));

        List<Operation> updates = getOperations();

        if (cfm.comparator.isDense())
        {
            if (prefix.isEmpty())
                throw new InvalidRequestException(String.format("Missing PRIMARY KEY part %s", cfm.clusteringColumns().get(0)));

            // An empty name for the compact value is what we use to recognize the case where there is not column
            // outside the PK, see CreateStatement.
            if (!cfm.compactValueColumn().name.bytes.hasRemaining())
            {
                // There is no column outside the PK. So no operation could have passed through validation
                assert updates.isEmpty();
                new Constants.Setter(cfm.compactValueColumn(), EMPTY).execute(key, cf, prefix, params);
            }
            else
            {
                // dense means we don't have a row marker, so don't accept to set only the PK. See CASSANDRA-5648.
                if (updates.isEmpty())
                    throw new InvalidRequestException(String.format("Column %s is mandatory for this COMPACT STORAGE table", cfm.compactValueColumn().name));

                for (Operation update : updates)
                    update.execute(key, cf, prefix, params);
            }
        }
        else
        {
            for (Operation update : updates)
                update.execute(key, cf, prefix, params);
        }

        SecondaryIndexManager indexManager = Keyspace.open(cfm.ksName).getColumnFamilyStore(cfm.cfId).indexManager;
        if (indexManager.hasIndexes())
        {
            for (Cell cell : cf)
            {
                // Indexed values must be validated by any applicable index. See CASSANDRA-3057/4240/8081 for more details
                if (!indexManager.validate(cell))
                    throw new InvalidRequestException(String.format("Can't index column value of size %d for index %s on %s.%s",
                                                                    cell.value().remaining(),
                                                                    cfm.getColumnDefinition(cell.name()).getIndexName(),
                                                                    cfm.ksName,
                                                                    cfm.cfName));
            }
        }
    }
#endif

    class parsed_insert : public modification_statement::parsed {
    private:
        const std::vector<::shared_ptr<column_identifier::raw>> _column_names;
        const std::vector<::shared_ptr<term::raw>> _column_values;
    public:
        /**
         * A parsed <code>INSERT</code> statement.
         *
         * @param name column family being operated on
         * @param columnNames list of column names
         * @param columnValues list of column values (corresponds to names)
         * @param attrs additional attributes for statement (CL, timestamp, timeToLive)
         */
        parsed_insert(std::experimental::optional<cf_name>&& name,
                      ::shared_ptr<attributes::raw> attrs,
                      std::vector<::shared_ptr<column_identifier::raw>> column_names,
                      std::vector<::shared_ptr<term::raw>> column_values,
                      bool if_not_exists)
            : modification_statement::parsed{std::move(name), attrs, conditions_vector{}, if_not_exists, false}
            , _column_names{column_names}
            , _column_values{column_values}
        { }
#if 0
        protected ModificationStatement prepareInternal(CFMetaData cfm, VariableSpecifications boundNames, Attributes attrs) throws InvalidRequestException
        {
            UpdateStatement stmt = new UpdateStatement(ModificationStatement.StatementType.INSERT,boundNames.size(), cfm, attrs);

            // Created from an INSERT
            if (stmt.isCounter())
                throw new InvalidRequestException("INSERT statement are not allowed on counter tables, use UPDATE instead");
            if (columnNames.size() != columnValues.size())
                throw new InvalidRequestException("Unmatched column names/values");
            if (columnNames.isEmpty())
                throw new InvalidRequestException("No columns provided to INSERT");

            for (int i = 0; i < columnNames.size(); i++)
            {
                ColumnIdentifier id = columnNames.get(i).prepare(cfm);
                ColumnDefinition def = cfm.getColumnDefinition(id);
                if (def == null)
                    throw new InvalidRequestException(String.format("Unknown identifier %s", id));

                for (int j = 0; j < i; j++)
                {
                    ColumnIdentifier otherId = columnNames.get(j).prepare(cfm);
                    if (id.equals(otherId))
                        throw new InvalidRequestException(String.format("Multiple definitions found for column %s", id));
                }

                Term.Raw value = columnValues.get(i);

                switch (def.kind)
                {
                    case PARTITION_KEY:
                    case CLUSTERING_COLUMN:
                        Term t = value.prepare(keyspace(), def);
                        t.collectMarkerSpecification(boundNames);
                        stmt.addKeyValue(def, t);
                        break;
                    default:
                        Operation operation = new Operation.SetValue(value).prepare(keyspace(), def);
                        operation.collectMarkerSpecification(boundNames);
                        stmt.addOperation(operation);
                        break;
                }
            }
            return stmt;
        }
#endif
    };

#if 0
    public static class ParsedUpdate extends ModificationStatement.Parsed
    {
        // Provided for an UPDATE
        private final List<Pair<ColumnIdentifier.Raw, Operation.RawUpdate>> updates;
        private final List<Relation> whereClause;

        /**
         * Creates a new UpdateStatement from a column family name, columns map, consistency
         * level, and key term.
         *
         * @param name column family being operated on
         * @param attrs additional attributes for statement (timestamp, timeToLive)
         * @param updates a map of column operations to perform
         * @param whereClause the where clause
         */
        public ParsedUpdate(CFName name,
                            Attributes.Raw attrs,
                            List<Pair<ColumnIdentifier.Raw, Operation.RawUpdate>> updates,
                            List<Relation> whereClause,
                            List<Pair<ColumnIdentifier.Raw, ColumnCondition.Raw>> conditions)
        {
            super(name, attrs, conditions, false, false);
            this.updates = updates;
            this.whereClause = whereClause;
        }

        protected ModificationStatement prepareInternal(CFMetaData cfm, VariableSpecifications boundNames, Attributes attrs) throws InvalidRequestException
        {
            UpdateStatement stmt = new UpdateStatement(ModificationStatement.StatementType.UPDATE, boundNames.size(), cfm, attrs);

            for (Pair<ColumnIdentifier.Raw, Operation.RawUpdate> entry : updates)
            {
                ColumnDefinition def = cfm.getColumnDefinition(entry.left.prepare(cfm));
                if (def == null)
                    throw new InvalidRequestException(String.format("Unknown identifier %s", entry.left));

                Operation operation = entry.right.prepare(keyspace(), def);
                operation.collectMarkerSpecification(boundNames);

                switch (def.kind)
                {
                    case PARTITION_KEY:
                    case CLUSTERING_COLUMN:
                        throw new InvalidRequestException(String.format("PRIMARY KEY part %s found in SET part", entry.left));
                    default:
                        stmt.addOperation(operation);
                        break;
                }
            }

            stmt.processWhereClause(whereClause, boundNames);
            return stmt;
        }
    }
#endif
};

}

}

#endif
