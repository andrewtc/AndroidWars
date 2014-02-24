/*
 * Author      : Matthew Johnson
 * Date        : 22/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{
	class Widget;
	class WidgetManager;


	class AbstractWidgetFactory
	{
	public:
		AbstractWidgetFactory() { }
		virtual ~AbstractWidgetFactory() { }

		virtual Widget* CreateWidget( WidgetManager* manager, const HashString& name ) const = 0;
	};


	template< class WidgetSubclass >
	class WidgetFactory : public AbstractWidgetFactory
	{
	public:
		WidgetFactory() { }
		virtual ~WidgetFactory() { }

		virtual Widget* CreateWidget( WidgetManager* manager, const HashString& name ) const;
	};


	class Widget
	{
		DECLARE_RTTI;

	protected:
		virtual void OnLoadFromXML( const XmlReader::XmlReaderIterator& xml );
		virtual void OnLoadFromDictionary( const Dictionary& dictionary );
		virtual void OnInit();

		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw( const Camera& camera );
		virtual bool OnPointerDown( float x, float y );
		virtual bool OnPointerUp( float x, float y );

	public:
		Widget( WidgetManager* manager, const HashString& name );
		Widget( const Widget& other );
		virtual Widget* Clone() const;
		virtual ~Widget();

		void Init();

		void Update( float elapsedTime );
		void Draw( const Camera& camera );

		WidgetManager* GetManager() const;

		void SetSprite( Sprite* sprite );
		Sprite* GetSprite() const;
		bool HasSprite() const;

		bool IsFixedSizeSprite() const;

		void SetWidth( float width );
		float GetWidth() const;
		void SetHeight( float height );
		float GetHeight() const;
		void SetSize( float width, float height );
		void SetSize( const Vec2f& size );
		Vec2f GetSize() const;

		template< class WidgetSubclass = Widget >
		WidgetSubclass* GetParent() const;

		// Get the Widget that is the base for this widget's hierarchy
		template< class WidgetSubclass = Widget >
		WidgetSubclass* GetRootWidget() const;

		bool Contains( float x, float y );

		void Show() { mIsVisible = true; }
		void Hide() { mIsVisible = false; }

		const HashString& GetName() const { return mName; }
		std::string GetFullName() const;

		void AddChild( Widget* child );
		void RemoveChild( Widget* child );
		void RemoveChildByName( const HashString& name );
		template< class WidgetSubclass = Widget >
		WidgetSubclass* GetChildByName( const HashString& name ) const;
		bool HasChild( const Widget* child ) const;
		bool HasChildWithName( const HashString& name ) const;

		void SetLayoutAbove( const HashString& widgetName );
		void SetLayoutBelow( const HashString& widgetName );
		void SetLayoutToLeftOf( const HashString& widgetName );
		void SetLayoutToRightOf( const HashString& widgetName );

		template< class WidgetSubclass = Widget >
		WidgetSubclass* FindSiblingByName( const HashString& name ) const;
		bool HasSibling( const Widget* sibling ) const;
		bool HasSiblingWithName( const HashString& siblingName ) const;

		template< class WidgetSubclass = Widget >
		WidgetSubclass* FindLayoutAbove() const;
		template< class WidgetSubclass = Widget >
		WidgetSubclass* FindLayoutBelow() const;
		template< class WidgetSubclass = Widget >
		WidgetSubclass* FindLayoutToLeftOf() const;
		template< class WidgetSubclass = Widget >
		WidgetSubclass* FindLayoutToRightOf() const;
		bool IsRelativeToSibling( const Widget* sibling ) const;
		bool IsRelativeToSiblingWithName( const HashString& siblingName ) const;

		bool IsInitialized() const;

		void SetOffset( const Vec2f& offset );
		Vec2f GetOffset() const;
		Vec2f FindPosition();
		const RectF& FindBounds();

		void SetVisible( bool isVisible );
		bool IsVisible() const;

	private:
		bool mIsInitialized;
		bool mNeedsBoundsUpdate;
		bool mIsFixedSizeSprite;
		bool mCenterInParent;
		bool mIsVisible;
		bool mDebugLayout;
		WidgetManager* mManager;
		Widget* mParent;
		Sprite* mSprite;
		RectF mCalculatedBounds;
		Vec2f mSize;
		Vec2f mOffset;
		Vec4f mMargins;
		HashString mName;
		HashString mBelowWidgetName;
		HashString mAboveWidgetName;
		HashString mToLeftOfWidgetName;
		HashString mToRightOfWidgetName;

	protected:
		void InvalidateBounds();
		void RecalculateBounds();
		void UpdateSiblingsOf( Widget* child );
		void SetSprite( const HashString& spriteName );
		void SetFixedSizeSprite( bool isFixedSizeSprite );

		Color mDrawColor;
		HashMap< Widget* > mChildren;

		enum Margin
		{
			MARGIN_LEFT,
			MARGIN_TOP,
			MARGIN_RIGHT,
			MARGIN_BOTTOM,
		};

		friend class WidgetManager;
	};
	//---------------------------------------
	template< class WidgetSubclass >
	Widget* WidgetFactory< WidgetSubclass >::CreateWidget( WidgetManager* manager, const HashString& name ) const
	{
		WidgetSubclass* derived = new WidgetSubclass( manager, name );
		Widget* base = dynamic_cast< Widget* >( derived );
		assertion( base, "Could not create Widget \"%s\" because subclass does not derive from Widget!", name.GetCString() );
		DebugPrintf( "Created \"%s\" (%s)", name.GetCString(), WidgetSubclass::TYPE.GetName(), base->GetWidth(), base->GetHeight() );
		return base;
	}
	//---------------------------------------
	template< class WidgetSubclass >
	WidgetSubclass* Widget::GetParent() const
	{
		return dynamic_cast< WidgetSubclass* >( mParent );
	}
	//---------------------------------------
	template< class WidgetSubclass >
	WidgetSubclass* Widget::GetRootWidget() const
	{
		Widget* current = this;

		while( current->mParent )
		{
			current = current->mParent;
		}

		return dynamic_cast< WidgetSubclass* >( current );
	}
	//---------------------------------------
	template< class WidgetSubclass >
	WidgetSubclass* Widget::GetChildByName( const HashString& name ) const
	{
		Widget* base = nullptr;

		// Look up the child Widget by its name.
		auto it = mChildren.find( name );

		if( it != mChildren.end() )
		{
			base = it->second;
		}

		return dynamic_cast< WidgetSubclass* >( base );
	}
	//---------------------------------------
	template< class WidgetSubclass >
	WidgetSubclass* Widget::FindSiblingByName( const HashString& name ) const
	{
		WidgetSubclass* result = nullptr;

		if( mParent != nullptr )
		{
			// Look up the sibling Widget by its name.
			result = mParent->GetChildByName< WidgetSubclass >( name );
		}

		return result;
	}
	//---------------------------------------
	template< class WidgetSubclass >
	WidgetSubclass* Widget::FindLayoutAbove() const
	{
		return FindSiblingByName< WidgetSubclass >( mAboveWidgetName );
	}
	//---------------------------------------
	template< class WidgetSubclass >
	WidgetSubclass* Widget::FindLayoutBelow() const
	{
		return FindSiblingByName< WidgetSubclass >( mBelowWidgetName );
	}
	//---------------------------------------
	template< class WidgetSubclass >
	WidgetSubclass* Widget::FindLayoutToLeftOf() const
	{
		return FindSiblingByName< WidgetSubclass >( mToLeftOfWidgetName );
	}
	//---------------------------------------
	template< class WidgetSubclass >
	WidgetSubclass* Widget::FindLayoutToRightOf() const
	{
		return FindSiblingByName< WidgetSubclass >( mToRightOfWidgetName );
	}
	//---------------------------------------
	inline bool Widget::IsInitialized() const
	{
		return mIsInitialized;
	}
}
