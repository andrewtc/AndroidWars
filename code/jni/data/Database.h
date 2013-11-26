#pragma once

namespace mage
{
	class TerrainType;
	class UnitType;
	class XmlReader;


	/**
	 * Abstract base class for basic Record type that can be indexed by a unique name.
	 */
	class Record
	{
	protected:
		Record( const HashString& name );

	public:
		virtual ~Record();

		HashString GetName() const;

	private:
		HashString mName;
	};


	inline HashString Record::GetName() const
	{
		return mName;
	}


	/**
	 * Factory class that creates and stores game data.
	 */
	template < class T >
	class Table
	{
	public:
		Table();
		virtual ~Table();

		void LoadRecordsFromFile( const char* file );
		T* CreateRecord( const HashString& name );
		void AddRecord( T* record );
		T* FindByName( const HashString& name );
		const T* FindByName( const HashString& name ) const;

	protected:
		virtual T* LoadRecordFromXml( XmlReader::XmlReaderIterator xmlIterator ) = 0;

		typedef HashMap< T* > RecordsByHashedName;
		RecordsByHashedName mRecords;
	};


	template < class T >
	Table< T >::Table() { }


	template < class T >
	Table< T >::~Table() { }


	template < class T >
	void Table< T >::LoadRecordsFromFile( const char* file )
	{
		// Open the terrain types file.
		XmlReader xmlReader( file );

		if( !xmlReader.Fail() )
		{
			XmlReader::XmlReaderIterator iterator;
			iterator = xmlReader.ReadRoot();

			while ( iterator.IsValid() )
			{
				// Try to load another record from the file.
				T* record = LoadRecordFromXml( iterator );

				if( record )
				{
					// If another record was loaded, add it to the table.
					AddRecord( record );
				}
				else
				{
					// If the last record was reached, stop loading records.
					break;
				}
			}
		}
	}


	template < class T >
	T* Table< T >::CreateRecord( const HashString& name )
	{
		// Make sure the name is valid and unique.
		assertion( name.GetHash() != 0, "Cannot create record without a valid name!" );

		// Create a new record with the specified name and add it to the Table.
		Record* record = new T( name );
		AddRecord( record );

		return record;
	}


	template < class T >
	void Table< T >::AddRecord( T* record )
	{
		// Make sure the record is valid.
		assertion( record, "Cannot add NULL record to Table!" );

		// Make sure a record with the same unique name doesn't already exist in this Table.
		HashString name = record->GetName();
		assertion( FindByName( name ) == nullptr, "Cannot create record because the name \"%s\" is not unique!", name.GetString().c_str() );

		// Index the record by its name.
		mRecords[ name ] = record;
	}


	template < class T >
	T* Table< T >::FindByName( const HashString& name )
	{
		T* result = nullptr;

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


	template < class T >
	const T* Table< T >::FindByName( const HashString& name ) const
	{
		T* result = nullptr;

		// Look up the record by name.
		typename RecordsByHashedName::const_iterator it;
		it = mRecords.find( name );

		if( it != mRecords.end() )
		{
			// If a record with the matching name was found, return it.
			result = *it;
		}

		return result;
	}


	/**
	 * Table that keeps track of TerrainTypes.
	 */
	class TerrainTypesTable : public Table< TerrainType >
	{
	public:
		TerrainTypesTable();
		virtual ~TerrainTypesTable();

	protected:
		virtual TerrainType* LoadRecordFromXml( XmlReader::XmlReaderIterator xmlIterator );
	};


	/**
	 * Table that keeps track of UnitTypes.
	 */
	class UnitTypesTable : public Table< UnitType >
	{
	public:
		UnitTypesTable();
		virtual ~UnitTypesTable();

	protected:
		virtual UnitType* LoadRecordFromXml( XmlReader::XmlReaderIterator xmlIterator );
	};


	/**
	 * Stores all game data.
	 */
	class Database
	{
	public:
		Database();
		~Database();

		TerrainTypesTable TerrainTypes;
		UnitTypesTable UnitTypes;
	};
}
