#pragma once


#define MAGE_TABLE_TEMPLATE \
	template < class TableType, class RecordType >

#define MAGE_TABLE \
	Table< TableType, RecordType >

#define MAGE_AUTO_GENERATE_TABLE_NAME( table, record ) \
	template<> const char* const Table< table, record >::TABLE_NAME = #record "s"; \
	template<> const char* const Table< table, record >::RECORD_NAME = #record;


namespace mage
{
	class Database;


	/**
	 * Factory class that creates and stores game data.
	 */
	MAGE_TABLE_TEMPLATE
	class Table
	{
	public:
		static const char* const TABLE_NAME;
		static const char* const RECORD_NAME;

		/**
		 * Abstract base class for basic Record type that can be indexed by a unique name.
		 */
		class Record
		{
		protected:
			Record( const HashString& name );

		public:
			~Record();

			Database* GetDatabase() const;
			TableType* GetTable() const;
			HashString GetName() const;
			const char* ToString() const;

		protected:
			TableType* mTable;
			const std::string mDebugName;
			const HashString mName;

		private:
			static std::string GenerateDebugName( const HashString& name );

			friend class MAGE_TABLE;
		};

		//static_assert( std::is_base_of< Record, T >::value, "Record type of Table must be derived from Table::Record." );

		virtual ~Table();

		void LoadRecordsFromXML( XmlReader::XmlReaderIterator rootIterator );
		RecordType* CreateRecord( const HashString& name );
		void AddRecord( RecordType* record );
		void DeleteAllRecords();

		RecordType* FindByName( const HashString& name );
		const RecordType* FindByName( const HashString& name ) const;

		Database* GetDatabase() const;

	protected:
		Table( Database* database );

		virtual void LoadRecordFromXml( RecordType* record, XmlReader::XmlReaderIterator elementIterator ) = 0;

		typedef HashMap< RecordType* > RecordsByHashedName;

		Database* mDatabase;
		RecordsByHashedName mRecords;
	};


	MAGE_TABLE_TEMPLATE
	MAGE_TABLE::Table( Database* database ) :
		mDatabase( database )
	{
		assertion( mDatabase, "Cannot create table without Database!" );
	}


	MAGE_TABLE_TEMPLATE
	MAGE_TABLE::~Table() { }


	MAGE_TABLE_TEMPLATE
	void MAGE_TABLE::LoadRecordsFromXML( XmlReader::XmlReaderIterator rootIterator )
	{
		assertion( rootIterator.IsValid(), "Could not load records from XML because iterator is invalid!" );

		// Search for the container element for this table.
		XmlReader::XmlReaderIterator outerIterator = rootIterator.NextChild( TABLE_NAME );
		assertion( outerIterator.IsValid(), "Could not load %s records from XML because no \"<%s>\" element was found!", RECORD_NAME, TABLE_NAME );

		// Go to the first XML element in the file.
		XmlReader::XmlReaderIterator elementIterator = outerIterator.NextChild( RECORD_NAME );

		while( elementIterator.IsValid() )
		{
			// Get the name of the TerrainType.
			HashString name = elementIterator.GetAttributeAsString( "name" );

			// Create a new record with the name.
			RecordType* record = CreateRecord( name );

			if( record )
			{
				// Load the record properties.
				DebugPrintf( "Loading %s \"%s\".", RECORD_NAME, name.GetString().c_str() );
				LoadRecordFromXml( record, elementIterator );
			}

			// Go to the next XML element in the file.
			elementIterator = elementIterator.NextSibling( RECORD_NAME );
		}
	}


	MAGE_TABLE_TEMPLATE
	RecordType* MAGE_TABLE::CreateRecord( const HashString& name )
	{
		// Make sure the name is valid and unique.
		assertion( name.GetHash() != 0, "Cannot create record without a valid name!" );

		// Create a new record with the specified name and add it to the Table.
		RecordType* record = new RecordType( name );
		AddRecord( record );

		return record;
	}


	MAGE_TABLE_TEMPLATE
	void MAGE_TABLE::AddRecord( RecordType* record )
	{
		// Make sure the record is valid.
		assertion( record, "Cannot add NULL record to Table!" );

		// Store a reference to this table in the record.
		assertion( record->mTable == nullptr, "Cannot add record to Table because it has already been stored in a Table!" );
		record->mTable = (TableType*)( this );

		// Make sure a record with the same unique name doesn't already exist in this Table.
		HashString name = record->GetName();
		assertion( FindByName( name ) == nullptr, "Cannot create record because the name \"%s\" is not unique!", name.GetString().c_str() );

		// Index the record by its name.
		mRecords[ name ] = record;
	}


	MAGE_TABLE_TEMPLATE
	void MAGE_TABLE::DeleteAllRecords()
	{
		for( auto it = mRecords.begin(); it != mRecords.end(); ++it )
		{
			// Delete each record in the table.
			delete it->second;
		}

		// Clear the list of records.
		mRecords.clear();
	}


	MAGE_TABLE_TEMPLATE
	RecordType* MAGE_TABLE::FindByName( const HashString& name )
	{
		RecordType* result = nullptr;

		// Look up the record by name.
		typename RecordsByHashedName::iterator it;
		it = mRecords.find( name );

		if( it != mRecords.end() )
		{
			// If a record with the matching name was found, return it.
			result = it->second;
		}

		return result;
	}


	MAGE_TABLE_TEMPLATE
	const RecordType* MAGE_TABLE::FindByName( const HashString& name ) const
	{
		RecordType* result = nullptr;

		// Look up the record by name.
		typename RecordsByHashedName::const_iterator it;
		it = mRecords.find( name );

		if( it != mRecords.end() )
		{
			// If a record with the matching name was found, return it.
			result = it->second;
		}

		return result;
	}


	MAGE_TABLE_TEMPLATE
	Database* MAGE_TABLE::GetDatabase() const
	{
		return mDatabase;
	}


	MAGE_TABLE_TEMPLATE
	MAGE_TABLE::Record::Record( const HashString& name )
		: mName( name )
		, mTable( nullptr )
		, mDebugName( GenerateDebugName( name ) )
	{ }


	MAGE_TABLE_TEMPLATE
	MAGE_TABLE::Record::~Record() { }


	MAGE_TABLE_TEMPLATE
	std::string MAGE_TABLE::Record::GenerateDebugName( const HashString& name )
	{
		// Format a debug name for the Record.
		static const int BUFFER_SIZE = 256;
		char buffer[ BUFFER_SIZE ];
		snprintf( buffer, BUFFER_SIZE, "%s \"%s\"", RECORD_NAME, name.GetCString() );

		// Return the newly formatted debug name.
		return buffer;
	}


	MAGE_TABLE_TEMPLATE
	Database* MAGE_TABLE::Record::GetDatabase() const
	{
		return mTable->GetDatabase();
	}


	MAGE_TABLE_TEMPLATE
	TableType* MAGE_TABLE::Record::GetTable() const
	{
		return mTable;
	}


	MAGE_TABLE_TEMPLATE
	HashString MAGE_TABLE::Record::GetName() const
	{
		return mName;
	}


	MAGE_TABLE_TEMPLATE
	const char* MAGE_TABLE::Record::ToString() const
	{
		return mDebugName.c_str();
	}
}

