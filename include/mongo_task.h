/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _MONGO_TASK_H_
#define _MONGO_TASK_H_

#include "common.h"
#include "bson.h"
#include "mongoc.h"

struct MongoResultSet
{
	bool is_success;
	bool is_one_table;
	vector<bson_t*> doc_list;

	MongoResultSet()
	{
		is_success = true;
		is_one_table = false;
	}

	~MongoResultSet()
	{
		for (size_t i = 0; i < doc_list.size(); i++)
		{
			bson_t* t = doc_list[i];
			bson_destroy(t);
		}
	}
};

enum MongoTaskType
{
	MONGO_TASK_TYPE_COMMAND_SIMPLE	= 1,
	MONGO_TASK_TYPE_CREATE_INDEX	= 2,
	MONGO_TASK_TYPE_DROP_INDEX		= 3,
	MONGO_TASK_TYPE_DROP			= 4,

	MONGO_TASK_TYPE_FIND_ONE		= 5,
	MONGO_TASK_TYPE_FIND			= 6,
	MONGO_TASK_TYPE_UPDATE_ONE		= 7,
	MONGO_TASK_TYPE_UPDATE_MANY		= 8,
	MONGO_TASK_TYPE_INSERT_ONE		= 9,
	MONGO_TASK_TYPE_INSERT_MANY		= 10,
	MONGO_TASK_TYPE_DELETE_ONE		= 11,
	MONGO_TASK_TYPE_DELETE_MANY		= 12,

	MONGO_TASK_TYPE_FIND_AND_MODIFY = 13,
};

struct MongoTask
{
	MongoTaskType task_type;
	HandleInfo handler;

	MongoTask()
	{

	}

	virtual ~MongoTask()
	{

	}
};

struct CommandSimpleMongoTask : MongoTask
{
	bson_t* command_bson;

	CommandSimpleMongoTask()
	{
		command_bson = NULL;
	}

	virtual ~CommandSimpleMongoTask()
	{
		
	}
};

struct CreateIndexMongoTask : MongoTask
{
	string table_name;
	bson_t* field_bson;
	bson_t* key_bson;

	CreateIndexMongoTask()
	{
		field_bson = NULL;
		key_bson = NULL;
	}

	virtual ~CreateIndexMongoTask()
	{
		if (field_bson)
		{
			bson_destroy(field_bson);
		}

		if (key_bson)
		{
			bson_destroy(key_bson);
		}
	}
};

struct DropIndexMongoTask : MongoTask
{
	string table_name;
	string index_name;

	DropIndexMongoTask()
	{

	}

	virtual ~DropIndexMongoTask()
	{

	}
};

struct DropMongoTask : MongoTask
{
	string	table_name;

	DropMongoTask()
	{

	}

	virtual ~DropMongoTask()
	{

	}
};

struct FindOneMongoTask : MongoTask
{
	string table_name;
	bson_t* query_bson;
	bson_t* fields_bson;

	uint32 query_flags;
	uint32 skip;
	uint32 limit;
	uint32 batch_size;

	FindOneMongoTask()
	{
		query_bson = NULL;
		fields_bson = NULL;

		query_flags = 0;
		skip = 0;
		limit = 0;
		batch_size = 0;
	}

	virtual ~FindOneMongoTask()
	{
		if (query_bson)
		{
			bson_destroy(query_bson);
		}

		if (fields_bson)
		{
			bson_destroy(fields_bson);
		}
	}
};

struct FindMongoTask : MongoTask
{
	string table_name;
	bson_t* query_bson;
	bson_t* fields_bson;
	
	uint32 query_flags;
	uint32 skip;
	uint32 limit;
	uint32 batch_size;

	FindMongoTask()
	{
		query_bson = NULL;
		fields_bson = NULL;

		query_flags = 0;
		skip = 0;
		limit = 0;
		batch_size = 0;
	}

	virtual ~FindMongoTask()
	{
		if (query_bson)
		{
			bson_destroy(query_bson);
		}

		if (fields_bson)
		{
			bson_destroy(fields_bson);
		}
	}
};

struct UpdateOneMongoTask : MongoTask
{
	string	table_name;
	bson_t*	filter_bson;
	bson_t* update_bson;

	UpdateOneMongoTask()
	{
		filter_bson = NULL;
		update_bson = NULL;
	}

	virtual ~UpdateOneMongoTask()
	{
		if (filter_bson)
		{
			bson_destroy(filter_bson);
		}

		if (update_bson)
		{
			bson_destroy(update_bson);
		}
	}
};

struct UpdateManyMongoTask : MongoTask
{
	string	table_name;
	bson_t* filter_bson;
	bson_t* update_bson;

	UpdateManyMongoTask()
	{
		filter_bson = NULL;
		update_bson = NULL;
	}

	virtual ~UpdateManyMongoTask()
	{
		if (filter_bson)
		{
			bson_destroy(filter_bson);
		}

		if (update_bson)
		{
			bson_destroy(update_bson);
		}
	}
};

struct InsertOneMongoTask : MongoTask
{
	string table_name;
	bson_t* doc_bson;

	InsertOneMongoTask()
	{
		doc_bson = NULL;
	}

	virtual ~InsertOneMongoTask()
	{
		if (doc_bson)
		{
			bson_destroy(doc_bson);
		}
	}
};

struct InsertManyMongoTask : MongoTask
{
	string	table_name;
	vector<bson_t*> doc_bson_list;
	bool ordered;

	InsertManyMongoTask()
	{
		ordered = true;
	}

	virtual ~InsertManyMongoTask()
	{
		for (uint32 n = 0; n < doc_bson_list.size(); n++)
		{
			bson_destroy(doc_bson_list[n]);
		}
	}
};

struct DeleteOneMongoTask : MongoTask
{
	string	table_name;
	bson_t* doc_bson;

	DeleteOneMongoTask()
	{
		doc_bson = NULL;
	}

	virtual ~DeleteOneMongoTask()
	{
		if (doc_bson)
		{
			bson_destroy(doc_bson);
		}
	}
};

struct DeleteManyMongoTask : MongoTask
{
	string	table_name;
	vector<bson_t*> doc_bson_list;

	DeleteManyMongoTask()
	{
		
	}

	virtual ~DeleteManyMongoTask()
	{
		for (uint32 n = 0; n < doc_bson_list.size(); n++)
		{
			bson_destroy(doc_bson_list[n]);
		}
	}
};

struct FindAndModifyMongoTask : MongoTask
{
	string table_name;
	bson_t* query_bson;
	bson_t* update_bson;

	FindAndModifyMongoTask()
	{
		query_bson = NULL;
		update_bson = NULL;
	}

	virtual ~FindAndModifyMongoTask()
	{
		if (query_bson)
		{
			bson_destroy(query_bson);
		}

		if (update_bson)
		{
			bson_destroy(update_bson);
		}
	}
};

#endif //_MONGO_TASK_H_