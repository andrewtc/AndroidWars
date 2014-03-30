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
	class WidgetTemplate;


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
		virtual void OnLoadFromTemplate( const WidgetTemplate& widgetTemplate );
		virtual void OnInit();

		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw( const Camera& camera );
		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );

	public:
		static const HashString HORIZONTAL_ALIGN_LEFT_NAME;
		static const HashString HORIZONTAL_ALIGN_CENTER_NAME;
		static const HashString HORIZONTAL_ALIGN_RIGHT_NAME;
		static const HashString VERTICAL_ALIGN_TOP_NAME;
		static const HashString VERTICAL_ALIGN_CENTER_NAME;
		static const HashString VERTICAL_ALIGN_BOTTOM_NAME;

		enum HorizontalAlignment
		{
			HORIZONTAL_ALIGN_LEFT,
			HORIZONTAL_ALIGN_CENTER,
			HORIZONTAL_ALIGN_RIGHT
		};

		enum VerticalAlignment
		{
			VERTICAL_ALIGN_TOP,
			VERTICAL_ALIGN_CENTER,
			VERTICAL_ALIGN_BOTTOM
		};

		Widget( WidgetManager* manager, const HashString& name );
		Widget( const Widget& other );
		virtual Widget* Clone() const;
		virtual ~Widget();

		void LoadFromTemplate( const WidgetTemplate& widgetTemplate );
		void Init();

		void Update( float elapsedTime );
		void Draw( const Camera& camera );
		bool PointerDown( const Pointer& pointer );
		bool PointerUp( const Pointer& pointer );

		WidgetManager* GetManager() const;

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
		template< class WidgetSubclass = Widget >
		void FindChildren( std::vector< WidgetSubclass* >& results );
		template< class WidgetSubclass = Widget >
		void FindDescendants( std::vector< WidgetSubclass* >& results );
		template< class WidgetSubclass = Widget >
		void FindChildrenMatching( Delegate< bool, WidgetSubclass* > test, std::vector< WidgetSubclass* >& results );
		template< class WidgetSubclass = Widget >
		void FindDescendantsMatching( Delegate< bool, WidgetSubclass* > test, std::vector< WidgetSubclass* >& results );
		template< class WidgetSubclass = Widget >
		void FindChildrenAt( float x, float y, std::vector< WidgetSubclass* >& results );
		template< class WidgetSubclass = Widget >
		void FindDescendantsAt( float x, float y, std::vector< WidgetSubclass* >& results );
		bool HasChild( const Widget* child ) const;
		bool HasChildWithName( const HashString& name ) const;

		template< class WidgetSubclass = Widget >
		WidgetSubclass* FindSiblingByName( const HashString& name ) const;
		bool HasSibling( const Widget* sibling ) const;
		bool HasSiblingWithName( const HashString& siblingName ) const;

		bool IsInitialized() const;

		void SetOffset( const Vec2f& offset );
		Vec2f GetOffset() const;
		Vec2f CalculatePosition();
		RectF CalculateBounds();
		RectF GetBoundsInParent() const;

		static HorizontalAlignment GetHorizontalAlignmentByName( const HashString& name );
		static VerticalAlignment GetVerticalAlignmentByName( const HashString& name );
		void SetHorizontalAlignment( HorizontalAlignment alignment );
		HorizontalAlignment GetHorizontalAlignment() const;
		void SetVerticalAlignment( VerticalAlignment alignment );
		VerticalAlignment GetVerticalAlignment() const;

		void SetVisibility( bool isVisible );
		void ToggleVisibility();
		bool GetVisibility() const;
		bool IsVisible() const;

		void SetDrawLayer( int drawLayer );
		int GetDrawLayer() const;

	private:
		bool mIsInitialized;
		bool mIsVisible;
		bool mPositionNeedsUpdate;
		bool mDrawOrderNeedsUpdate;
		bool mDebugLayout;
		int mDrawLayer;
		HorizontalAlignment mHorizontalAlignment;
		VerticalAlignment mVerticalAlignment;
		WidgetManager* mManager;
		Widget* mParent;
		Vec2f mCalculatedPosition;
		Vec2f mSize;
		Vec2f mOffset;
		HashString mName;
		std::map< int, std::vector< Widget* > > mChildrenByDrawLayer;

	protected:
		void InvalidatePosition();
		void UpdatePosition();
		void InvalidateDrawOrder();
		void UpdateDrawOrder();

		HashMap< Widget* > mChildren;

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
	void Widget::FindChildren( std::vector< WidgetSubclass* >& results )
	{
		// Find all children in draw order.
		FindChildrenMatching< WidgetSubclass >( []( WidgetSubclass* ) -> bool
		{
			return true;
		},
		results );
	}
	//---------------------------------------
	template< class WidgetSubclass >
	void Widget::FindDescendants( std::vector< WidgetSubclass* >& results )
	{
		// Find all descendants in draw order.
		FindDescendantsMatching< WidgetSubclass >( []( WidgetSubclass* ) -> bool
		{
			return true;
		},
		results );
	}
	//---------------------------------------
	template< class WidgetSubclass >
	void Widget::FindChildrenMatching( Delegate< bool, WidgetSubclass* > test, std::vector< WidgetSubclass* >& results )
	{
		assertion( test.IsValid(), "Cannot query children of Widget \"%s\" because no valid callback function was supplied!", mName.GetCString() );

		// Determine the draw order of all children.
		UpdateDrawOrder();

		for( auto layerIt = mChildrenByDrawLayer.begin(); layerIt != mChildrenByDrawLayer.end(); ++layerIt )
		{
			// Process layers in order (i.e. from bottom to top).
			const std::vector< Widget* >& children = layerIt->second;

			for( auto childIt = children.begin(); childIt != children.end(); ++childIt )
			{
				// Check children from bottom to top and find any that match the class being queried.
				WidgetSubclass* derived = dynamic_cast< WidgetSubclass* >( *childIt );

				if( derived != nullptr && test.Invoke( derived ) )
				{
					// If the child is of the right type, add it to the list of results.
					results.push_back( derived );
				}
			}
		}
	}
	//---------------------------------------
	template< class WidgetSubclass >
	void Widget::FindDescendantsMatching( Delegate< bool, WidgetSubclass* > test, std::vector< WidgetSubclass* >& results )
	{
		assertion( test.IsValid(), "Cannot query descendants of Widget \"%s\" because no valid callback function was supplied!", mName.GetCString() );

		// Determine the draw order of all children.
		UpdateDrawOrder();

		for( auto layerIt = mChildrenByDrawLayer.begin(); layerIt != mChildrenByDrawLayer.end(); ++layerIt )
		{
			// Process layers in order (i.e. from bottom to top).
			const std::vector< Widget* >& children = layerIt->second;

			for( auto childIt = children.begin(); childIt != children.end(); ++childIt )
			{
				// Check children in draw order and find any that match the class being queried.
				Widget* child = ( *childIt );

				WidgetSubclass* derivedChild = dynamic_cast< WidgetSubclass* >( child );

				if( derivedChild != nullptr && test.Invoke( derivedChild ) )
				{
					// If the child is of the right type, add it to the list of results.
					results.push_back( derivedChild );
				}

				// Let the child add its own children to the list in draw order.
				child->FindDescendantsMatching< WidgetSubclass >( test, results );
			}
		}
	}
	//---------------------------------------
	template< class WidgetSubclass >
	void Widget::FindChildrenAt( float x, float y, std::vector< WidgetSubclass* >& results )
	{
		// Find all children whose bounds contain the point.
		FindChildrenMatching< WidgetSubclass >( [ x, y ]( WidgetSubclass* widget ) -> bool
		{
			return widget->CalculateBounds().contains( x, y );
		},
		results );
	}
	//---------------------------------------
	template< class WidgetSubclass >
	void Widget::FindDescendantsAt( float x, float y, std::vector< WidgetSubclass* >& results )
	{
		// Find all children whose bounds contain the point.
		FindDescendantsMatching< WidgetSubclass >( [ x, y ]( WidgetSubclass* widget ) -> bool
		{
			return widget->CalculateBounds().Contains( x, y );
		},
		results );
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
	inline bool Widget::IsInitialized() const
	{
		return mIsInitialized;
	}
}
