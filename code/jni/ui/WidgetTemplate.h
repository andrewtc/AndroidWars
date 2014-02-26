#pragma once

namespace mage
{
	/**
	 * Stores information about a Widget that has not been created yet.
	 */
	class WidgetProperties
	{
	public:
		WidgetProperties();
		~WidgetProperties();

		void SetName( const HashString& name );
		HashString GetName() const;

		template< typename T >
		void SetProperty( const HashString& name, const T& value );
		template< typename T >
		void GetProperty( const HashString& name, T& result ) const;

		void AddChild( WidgetProperties* child );
		WidgetProperties* RemoveChildByName( const HashString& name );
		void DestroyChildByName( const HashString& name );
		void DestroyAllChildren();

	protected:
		HashString mName;
		Dictionary mProperties;
		HashMap< WidgetProperties* > mChildren;
	};


	template< typename T >
	void WidgetProperties::SetProperty( const HashString& name, const T& value )
	{
		mProperties.Set( name, value );
	}


	template< typename T >
	void WidgetProperties::GetProperty( const HashString& name, T& result ) const
	{
		mProperties.Get( name, result );
	}


	/**
	 * Encapsulates a nested list of properties that can be used to construct a
	 * hierarchy of Widgets.
	 */
	class WidgetTemplate
	{
	public:
		WidgetManager* GetManager() const;
		HashString GetName() const;

		WidgetProperties& GetRoot();
		const WidgetProperties& GetRoot() const;

	private:
		WidgetTemplate( WidgetManager* manager, const HashString& name );
		~WidgetTemplate();

		WidgetManager* mManager;
		const HashString& mName;
		WidgetProperties mRoot;

		friend class WidgetManager;
	};
}
