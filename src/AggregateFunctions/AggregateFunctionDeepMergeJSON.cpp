#include <AggregateFunctions/AggregateFunctionDeepMergeJSON.h>
#include <AggregateFunctions/AggregateFunctionFactory.h>
#include <AggregateFunctions/FactoryHelpers.h>
#include <Core/Field.h>
#include <DataTypes/DataTypesBinaryEncoding.h>
#include <DataTypes/Serializations/SerializationObject.h>
#include <IO/ReadBufferFromMemory.h>
#include <IO/ReadBufferFromString.h>
#include <IO/WriteBufferFromString.h>

#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-function"

namespace DB
{

namespace ErrorCodes
{
extern const int NUMBER_OF_ARGUMENTS_DOESNT_MATCH;
extern const int TOO_LARGE_ARRAY_SIZE;
extern const int TOO_LARGE_STRING_SIZE;
}

namespace
{
/// Helper to validate path length
void validatePathLength(size_t path_size)
{
    if (path_size > MAX_JSON_MERGE_PATH_LENGTH)
        throw Exception(
            ErrorCodes::TOO_LARGE_STRING_SIZE, "JSON path too long: {} bytes (maximum: {})", path_size, MAX_JSON_MERGE_PATH_LENGTH);
}

/// Helper to validate paths count
void validatePathsCount(size_t paths_count)
{
    if (paths_count > MAX_JSON_MERGE_PATHS)
        throw Exception(
            ErrorCodes::TOO_LARGE_ARRAY_SIZE, "Too many paths in JSON merge: {} (maximum: {})", paths_count, MAX_JSON_MERGE_PATHS);
}

/// Helper to validate total size
void validateTotalSize(size_t total_size)
{
    if (total_size > MAX_JSON_MERGE_TOTAL_SIZE)
        throw Exception(
            ErrorCodes::TOO_LARGE_STRING_SIZE,
            "JSON merge state size too large: {} bytes (maximum: {} bytes)",
            total_size,
            MAX_JSON_MERGE_TOTAL_SIZE);
}
}

bool DeepMergeJSONAggregateData::isObjectPath(const StringRef & path) const
{
    /*auto it = paths.upper_bound(path);
    return it != paths.end() && it->first.size > path.size && memcmp(it->first.data, path.data, path.size) == 0
        && it->first.data[path.size] == '.';
        */
    return false;
}

bool DeepMergeJSONAggregateData::handleDeletion(const StringRef & target_path, Arena * arena)
{
    /*
    auto it = paths.find(target_path);

    if (it == paths.end())
    {
        // Need to intern the string for new deletion marker
        char * data = arena->alloc(target_path.size);
        memcpy(data, target_path.data, target_path.size);
        paths[StringRef(data, target_path.size)] = PathData{Field(), true};
    }
    else
    {
        it->second.value = Field();
        it->second.is_deleted = true;
    }

    removeChildPaths(target_path);
    */
    return true;
}

void DeepMergeJSONAggregateData::addPath(const StringRef & path, const Field & value, Arena *)
{
    /*
    auto it = paths.find(path);
    if (it != paths.end())
    {
        object.insert(value);
    }
    else
    {
        paths[path] = PathData{value, false};
    }

    /// Remove child paths if this is now a leaf value (non-object)
    if (!value.isNull() && value.getType() != Field::Types::Object)
        removeChildPaths(path);
        */
}

void DeepMergeJSONAggregateData::removeChildPaths(const StringRef & parent_path)
{
    /*
    String prefix = parent_path.toString() + ".";
    auto it = paths.lower_bound(StringRef(prefix));

    for (auto it = object.typed_paths.begin(); it != paths.end();)
    {
        //if (it->first.size >= prefix.size() && memcmp(it->first.data, prefix.data(), prefix.size()) == 0)
        if (it->first.starts_with(prefix)) {
            it = typed_paths.erase(it);
        } else {
            it++;
        }
    }

    for (auto it = object.dynamic_paths.begin(); it != paths.end();)
    {
        //if (it->first.size >= prefix.size() && memcmp(it->first.data, prefix.data(), prefix.size()) == 0)
        if (it->first.starts_with(prefix)) {
            it = dynamic_paths.erase(it);
        } else {
            it++;
        }
    }
    */
}

void AggregateFunctionDeepMergeJSON::add(AggregateDataPtr __restrict place, const IColumn ** columns, size_t row_num, Arena * arena) const
{
    auto & aggregate_data = data(place);
    const auto & col_object = assert_cast<const ColumnObject &>(*columns[0]);

    aggregate_data.object = const_cast<ColumnObject*>(&col_object);
  //  processColumnObject(col_object, row_num, aggregate_data, arena);
}

void AggregateFunctionDeepMergeJSON::processPath(
    const StringRef & path, const Field & value, DeepMergeJSONAggregateData & aggregate_data, Arena * arena) const
{
    validatePathLength(path.size);

    /// Check for deletion suffix if deletion key is configured
    if (deletion_key.has_value())
    {
        std::string path_str = path.toString();
        if (path_str.ends_with(std::string(".") + *deletion_key) && value.getType() == Field::Types::Bool && value.safeGet<bool>())
        {
            std::string target_path = path_str.substr(0, path_str.size() - (*deletion_key).size() - 1);
            aggregate_data.handleDeletion(StringRef(target_path), arena);
            return;
        }
    }

    auto interned_path = internString(path, arena);
    aggregate_data.addPath(interned_path, value, arena);
}

void AggregateFunctionDeepMergeJSON::processColumnObject(
    const ColumnObject & col_object, size_t row_num, DeepMergeJSONAggregateData & aggregate_data, Arena * arena) const
{
    /*
    /// Process typed paths

    /// Process dynamic paths

    /// Process shared data
    const auto [shared_data_paths, shared_data_values] = col_object.getSharedDataPathsAndValues();
    const auto & shared_data_offsets = col_object.getSharedDataOffsets();
    size_t start = shared_data_offsets[static_cast<ssize_t>(row_num) - 1];
    size_t end = shared_data_offsets[static_cast<ssize_t>(row_num)];

    for (size_t i = start; i < end; ++i)
    {
        auto path = shared_data_paths->getDataAt(i);
        validatePathLength(path.size);

        /// Deserialize value from shared data
        auto value_data = shared_data_values->getDataAt(i);
        ReadBufferFromMemory buf(value_data.data, value_data.size);
        auto type = decodeDataType(buf);

        const auto column = type->createColumn();
        type->getDefaultSerialization()->deserializeBinary(*column, buf, FormatSettings());

        Field value;
        column->get(0, value);
        processPath(path, value, aggregate_data, arena);
    }

    validatePathsCount(aggregate_data.paths.size());
    */
}

void AggregateFunctionDeepMergeJSON::merge(AggregateDataPtr __restrict place, ConstAggregateDataPtr rhs, Arena * arena) const
{
    auto & aggregate_data = data(place);
    const auto & rhs_data = data(rhs);
    if (aggregate_data.object == nullptr) {
        aggregate_data.object = rhs_data.object;
    } else if (rhs_data.object != nullptr) {
        auto rhs_object = rhs_data.object;
        aggregate_data.object->insertRangeFrom(*rhs_object, 0, rhs_object->size());
    }

    /*
    /// Merge paths from rhs, treating them as latest values
    for (const auto & [path, path_data] : rhs_data.paths)
    {
        auto interned_path = internString(path, arena);
        if (path_data.is_deleted)
        {
            aggregate_data.handleDeletion(interned_path, arena);
        }
        else
        {
            aggregate_data.addPath(interned_path, path_data.value, arena);
        }
    }

    validatePathsCount(aggregate_data.paths.size());
    */
}

void AggregateFunctionDeepMergeJSON::serialize(
    ConstAggregateDataPtr __restrict place, WriteBuffer & buf, [[maybe_unused]] std::optional<size_t> version) const
{
    const auto & aggregate_data = data(place);
    if (aggregate_data.object == nullptr) return;
    DataTypeObject dto(DataTypeObject::SchemaFormat::JSON);
    auto serialization = dto.getDefaultSerialization();
    Field field = (*aggregate_data.object)[0];
    serialization->serializeBinary(field, buf, {});
}

void AggregateFunctionDeepMergeJSON::deserialize(
    AggregateDataPtr __restrict place, ReadBuffer & buf, [[maybe_unused]] std::optional<size_t> version, Arena * arena) const
{
    auto & aggregate_data = data(place);
    if (buf.available() == 0) {
        aggregate_data.object = nullptr;
    }
    DataTypeObject dto(DataTypeObject::SchemaFormat::JSON);
    auto serialization = dto.getDefaultSerialization();
    Field field = (*aggregate_data.object)[0];
    serialization->deserializeBinary(field, buf, {});
}

void AggregateFunctionDeepMergeJSON::insertResultInto(AggregateDataPtr __restrict place, IColumn & to, [[maybe_unused]] Arena * arena) const
{
    const auto & aggregate_data = data(place);
    auto & col_object = assert_cast<ColumnObject &>(to);
    if (aggregate_data.object != nullptr && aggregate_data.object->size() > 0) {
        size_t lastIndex = aggregate_data.object->size();
        for (const auto & [path, column] : aggregate_data.object->getTypedPaths())
            col_object.getTypedPaths()[path]->insertFrom(*column, lastIndex - 1);

        for (const auto & [path, column] : aggregate_data.object->getDynamicPaths())
        {
            /// Output only non-null values from dynamic paths. We cannot distinguish cases when
            /// dynamic path has Null value and when it's absent in the row and consider them equivalent.
            for (size_t n = lastIndex - 1; n != SIZE_MAX; n--)
            {
                if (!column->isNullAt(n)) {
                    if (auto it = col_object.getDynamicPaths().find(path); it != col_object.getDynamicPaths().end())
                        it->second->insertFrom(*column, n);
                    /// Try to add a new dynamic path.
                    else if (auto * dynamic_path_column = col_object.tryToAddNewDynamicPath(path))
                        dynamic_path_column->insertFrom(*column, n);
                    /// Limit on dynamic paths is reached, add path to shared data later.
                    // else
                    //    src_dynamic_paths_for_shared_data.push_back(path);
                    break;
                }
                if (n == 0) break;
            }
        }

        // TODO shared data
        //to.insertRangeFrom(*aggregate_data.object, 0, );
        // to.insert((*aggregate_data.object)[0]);
        to.insertRangeFrom(*aggregate_data.object, 0, 1);
    } else {
        to.insertDefault();
    }
}

void AggregateFunctionDeepMergeJSON::addBatchSinglePlace(
    size_t row_begin, size_t row_end, AggregateDataPtr __restrict place, const IColumn ** columns, Arena * arena, ssize_t if_argument_pos)
    const
{
    if (if_argument_pos >= 0)
    {
        IAggregateFunctionDataHelper<DeepMergeJSONAggregateData, AggregateFunctionDeepMergeJSON>::addBatchSinglePlace(
            row_begin, row_end, place, columns, arena, if_argument_pos);
        return;
    }

    const auto & col_object = assert_cast<const ColumnObject &>(*columns[0]);
    auto & aggregate_data = data(place);

    if (aggregate_data.object == nullptr) {
        aggregate_data.object = new ColumnObject(col_object);
    } else {
        aggregate_data.object->insertRangeFrom(col_object, row_begin, row_end - row_begin);
    }
}

void AggregateFunctionDeepMergeJSON::addManyDefaults(
    AggregateDataPtr __restrict /*place*/, const IColumn ** /*columns*/, size_t /*length*/, Arena * /*arena*/) const
{
    /// Default value for JSON is empty object, so nothing to add
}

namespace
{

AggregateFunctionPtr
createAggregateFunctionDeepMergeJSON(const std::string & name, const DataTypes & argument_types, const Array & parameters, const Settings *)
{
    if (argument_types.size() != 1)
        throw Exception(ErrorCodes::NUMBER_OF_ARGUMENTS_DOESNT_MATCH, "Aggregate function {} requires exactly one argument", name);

    return std::make_shared<AggregateFunctionDeepMergeJSON>(argument_types, parameters);
}

}

void registerAggregateFunctionDeepMergeJSON(AggregateFunctionFactory & factory)
{
    factory.registerFunction("deepMergeJSON", createAggregateFunctionDeepMergeJSON);
}

}
