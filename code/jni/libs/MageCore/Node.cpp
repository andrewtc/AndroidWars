#include "CoreLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( ControlledObject, Node );

//---------------------------------------
Node::Node()
	: ControlledObject( "Node" )
	, WorldTransformIsCurrent( false )
	, mParent( 0 )
	, Visualizer( 0 )
{}
//---------------------------------------
Node::Node( const std::string& name )
	: ControlledObject( name )
	, WorldTransformIsCurrent( false )
	, mParent( 0 )
	, Visualizer( 0 )
{}
//---------------------------------------
Node::~Node()
{
	for ( child_iterator itr = mChildren.begin(); itr != mChildren.end(); ++itr )
	{
		Node* n = *itr;

		n->mParent = 0;
	}
}
//---------------------------------------
void Node::AttachChild( Node* child )
{
	if ( child->GetParent() )
	{
		WarnInfo( "Node::AttachChild( child ) : child node already has a parent!\n", "" );
		return;
	}

	child->mParent = this;
	mChildren.push_back( child );
}
//---------------------------------------
void Node::DetachChild( Node* child )
{
	if ( mChildren.remove_elem( child ) )
	{
		child->mParent = 0;
	}
}
//---------------------------------------
void Node::RemoveFromHeirarchy()
{
	if ( !mParent )
	{
		WarnInfo( "Node::RemoveFromHeirarchy() : Called on root node. Children will be loose.\n", "" );
	}
	for ( child_iterator itr = mChildren.begin(); itr != mChildren.end(); ++itr )
	{
		Node* child = *itr;

		child->mParent = 0;
		if ( mParent )
		{
			mParent->AttachChild( child );
		}
	}
	if ( mParent )
	{
		mParent->DetachChild( this );
	}
	mChildren.clear();
}
//---------------------------------------
Node* Node::GetChildAt( int i )
{
	if ( i < 0 || i >= (int) mChildren.size() )
	{
		WarnInfo( "Node::GetChildAt( i ) : index out of bounds! %d\n", i );
		return 0;
	}
	return mChildren[i];
}
//---------------------------------------
void Node::OnUpdate( float currentTime )
{
	UpdateControllers( currentTime );

	if ( !WorldTransformIsCurrent )
	{
		if ( mParent )
		{
			WorldTransform = mParent->WorldTransform * LocalTransform;
		}
		else
		{
			WorldTransform = LocalTransform;
		}
	}

	for ( child_iterator itr = mChildren.begin(); itr != mChildren.end(); ++itr )
	{
		Node* n = *itr;

		n->OnUpdate( currentTime );
	}
}
//---------------------------------------
void Node::OnDraw()
{
	for ( child_iterator itr = mChildren.begin(); itr != mChildren.end(); ++itr )
	{
		Node* n = *itr;

		n->OnDraw();
	}

	if ( Visualizer )
	{
		Visualizer->OnDraw( this );
	}
}
//---------------------------------------
Vec2f Node::WorldToLocal( const Vec2f& worldPos )
{
	Vec2f pos = worldPos;
	if ( mParent )
	{
		pos -= mParent->WorldTransform.GetTranslation();
	}
	return pos;
}
//---------------------------------------
Vec2f Node::LocalToWorld( const Vec2f& localPos )
{
	Vec2f pos = localPos;
	if ( mParent )
	{
		pos += mParent->WorldTransform.GetTranslation();
	}
	return pos;
}
//---------------------------------------