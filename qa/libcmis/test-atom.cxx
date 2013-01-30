/* libcmis
 * Version: MPL 1.1 / GPLv2+ / LGPLv2+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2011 SUSE <cbosdonnat@suse.com>
 *
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPLv2+"), or
 * the GNU Lesser General Public License Version 2 or later (the "LGPLv2+"),
 * in which case the provisions of the GPLv2+ or the LGPLv2+ are applicable
 * instead of those above.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

#include "atom-document.hxx"
#include "atom-folder.hxx"
#include "atom-session.hxx"
#include "test-helpers.hxx"

// InMemory local test server data
#define SERVER_ATOM_URL string( "http://localhost:8080/inmemory/atom" )
#define SERVER_REPOSITORIES_COUNT list< string >::size_type( 1 )
#define SERVER_REPOSITORY string( "A1" )
#define SERVER_USERNAME string( "tester" )
#define SERVER_PASSWORD string( "somepass" )

#define TEST_DOCUMENT_ID string( "116" )
#define TEST_DOCUMENT_TYPE string( "text/plain" )
#define TEST_SAMPLE_CONTENT string( "Some sample text to upload" )
#define TEST_DOCUMENT_PARENTS_COUNT vector< libcmis::FolderPtr >::size_type( 1 )
#define TEST_DOCUMENT_PARENT string( "101" )

#define TEST_CHILDREN_FOLDER_COUNT 2
#define TEST_CHILDREN_DOCUMENT_COUNT 3
#define TEST_CHILDREN_COUNT vector<libcmis::ObjectPtr>::size_type( TEST_CHILDREN_FOLDER_COUNT + TEST_CHILDREN_DOCUMENT_COUNT )

using boost::shared_ptr;
using namespace std;

class AtomTest : public CppUnit::TestFixture
{
    public:

        // Node operations tests

        void getChildrenTest( );
        void getObjectParentsTest( );
        void getContentStreamTest( );
        void setContentStreamTest( );
        void updatePropertiesTest( );
        void createFolderTest( );
        void createFolderBadTypeTest( );
        void dumpDocumentToXmlTest( );
        void createDocumentTest( );
        void deleteDocumentTest( );
        void deleteTreeTest( );
        void checkOutTest( );
        void cancelCheckOutTest( );
        void checkInTest( );
        void getAllVersionsTest( );
        void moveTest( );

        CPPUNIT_TEST_SUITE( AtomTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST( getObjectParentsTest );
        CPPUNIT_TEST( getContentStreamTest );
        CPPUNIT_TEST( setContentStreamTest );
        CPPUNIT_TEST( updatePropertiesTest );
        CPPUNIT_TEST( createFolderTest );
        CPPUNIT_TEST( createFolderBadTypeTest );
        CPPUNIT_TEST( createDocumentTest );
        CPPUNIT_TEST( deleteDocumentTest );
        CPPUNIT_TEST( deleteTreeTest );
        CPPUNIT_TEST( checkOutTest );
        CPPUNIT_TEST( cancelCheckOutTest );
        CPPUNIT_TEST( checkInTest );
        CPPUNIT_TEST( getAllVersionsTest );
        CPPUNIT_TEST( moveTest );
        CPPUNIT_TEST_SUITE_END( );
};

void AtomTest::getChildrenTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::FolderPtr folder = session.getRootFolder( );

    vector< libcmis::ObjectPtr > children = folder->getChildren( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children", TEST_CHILDREN_COUNT, children.size() );

    int folderCount = 0;
    int documentCount = 0;
    for ( vector< libcmis::ObjectPtr >::iterator it = children.begin( );
          it != children.end( ); ++it )
    {
        if ( NULL != dynamic_cast< AtomFolder* >( it->get() ) )
            ++folderCount;
        else if ( NULL != dynamic_cast< AtomDocument* >( it->get() ) )
            ++documentCount;
    }
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of folder children",
            TEST_CHILDREN_FOLDER_COUNT, folderCount );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of document children",
            TEST_CHILDREN_DOCUMENT_COUNT, documentCount );
}

void AtomTest::getObjectParentsTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::ObjectPtr object = session.getObject( TEST_DOCUMENT_ID );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );
    vector< libcmis::FolderPtr > actual = document->getParents( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad number of parents",
           TEST_DOCUMENT_PARENTS_COUNT, actual.size() );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent ID",
            TEST_DOCUMENT_PARENT, actual.front( )->getId( ) );
}

void AtomTest::getContentStreamTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::ObjectPtr object = session.getObject( TEST_DOCUMENT_ID );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );

    try
    {
        shared_ptr< istream >  is = document->getContentStream( );
        CPPUNIT_ASSERT_MESSAGE( "Content stream should be returned", NULL != is.get() );
        CPPUNIT_ASSERT_MESSAGE( "Non-empty content stream should be returned", is->good() && !is->eof() );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void AtomTest::setContentStreamTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::ObjectPtr object = session.getObject( TEST_DOCUMENT_ID );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );

    try
    {
        boost::shared_ptr< ostream > os ( new stringstream ( TEST_SAMPLE_CONTENT ) );
        string filename( "name.txt" );
        document->setContentStream( os, TEST_DOCUMENT_TYPE, filename );
        
        CPPUNIT_ASSERT_MESSAGE( "Object not refreshed during setContentStream", object->getRefreshTimestamp( ) > 0 );

        // Get the new content to check is has been properly uploaded
        shared_ptr< istream > newIs = document->getContentStream( );
        stringstream is;
        is << newIs->rdbuf();
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded",
                TEST_SAMPLE_CONTENT, is.str() );
    
        // Testing other values like LastModifiedBy or LastModificationTime
        // is server dependent... don't do it. 
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Node not properly refreshed",
                ( long )TEST_SAMPLE_CONTENT.size(), document->getContentLength() );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void AtomTest::updatePropertiesTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );

    // Values for the test
    libcmis::ObjectPtr object = session.getObject( "114" );
    string propertyName( "cmis:name" );
    string expectedValue( "New name" );

    // Fill the map of properties to change
    map< string, libcmis::PropertyPtr > newProperties;

    libcmis::ObjectTypePtr objectType = object->getTypeDescription( );
    map< string, libcmis::PropertyTypePtr >::iterator it = objectType->getPropertiesTypes( ).find( propertyName );
    vector< string > values;
    values.push_back( expectedValue );
    libcmis::PropertyPtr property( new libcmis::Property( it->second, values ) );
    newProperties[ propertyName ] = property;

    // Update the properties (method to test)
    libcmis::ObjectPtr updated = object->updateProperties( newProperties );

    // Checks
    map< string, libcmis::PropertyPtr >::iterator propIt = updated->getProperties( ).find( propertyName );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong value after refresh", expectedValue, propIt->second->getStrings().front( ) );
}

void AtomTest::createFolderTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::FolderPtr parent = session.getFolder( session.getRootId( ) );

    // Prepare the properties for the new object, object type is cmis:folder
    map< string, libcmis::PropertyPtr > props;
    libcmis::ObjectTypePtr type = session.getType( "cmis:folder" );
    map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

    // Set the object name
    map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:name property type not found on parent type", it != propTypes.end( ) );
    vector< string > nameValues;
    nameValues.push_back( "createFolderTest" );
    libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );
   
    // set the object type 
    it = propTypes.find( string( "cmis:objectTypeId" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:objectTypeId property type not found on parent type", it != propTypes.end( ) );
    vector< string > typeValues;
    typeValues.push_back( "cmis:folder" );
    libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

    // Actually send the folder creation request
    libcmis::FolderPtr created = parent->createFolder( props );

    // Check that something came back
    CPPUNIT_ASSERT_MESSAGE( "Change token shouldn't be empty: object should have been refreshed",
            !created->getChangeToken( ).empty() );
}

void AtomTest::createFolderBadTypeTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::FolderPtr parent = session.getFolder( session.getRootId( ) );

    // Prepare the properties for the new object, object type is cmis:document to trigger the exception
    map< string, libcmis::PropertyPtr > props;
    libcmis::ObjectTypePtr type = session.getType( "cmis:document" );
    map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

    // Set the object name
    map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:name property type not found on parent type", it != propTypes.end( ) );
    vector< string > nameValues;
    nameValues.push_back( "createFolderBadTypeTest" );
    libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );

    // Set the object type 
    it = propTypes.find( string( "cmis:objectTypeId" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:objectTypeId property type not found on parent type", it != propTypes.end( ) );
    vector< string > typeValues;
    typeValues.push_back( "cmis:document" );
    libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

    // Actually send the folder creation request
    try
    {
        libcmis::FolderPtr created = parent->createFolder( props );
        CPPUNIT_FAIL( "Should not succeed to return a folder" );
    }
    catch ( libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_MESSAGE( "Bad exception message",
                string( e.what( ) ).find( "Created object is not a folder: " ) != string::npos );
    }
}

void AtomTest::createDocumentTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::FolderPtr parent = session.getFolder( session.getRootId( ) );

    // Prepare the properties for the new object, object type is cmis:folder
    map< string, libcmis::PropertyPtr > props;
    libcmis::ObjectTypePtr type = session.getType( "cmis:document" );
    map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

    // Set the object name
    map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:name property type not found on parent type", it != propTypes.end( ) );
    vector< string > nameValues;
    nameValues.push_back( "createDocumentTest" );
    libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );
   
    // set the object type 
    it = propTypes.find( string( "cmis:objectTypeId" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:objectTypeId property type not found on parent type", it != propTypes.end( ) );
    vector< string > typeValues;
    typeValues.push_back( "cmis:document" );
    libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

    // Actually send the document creation request
    string contentStr = "Some content";
    boost::shared_ptr< ostream > os ( new stringstream( contentStr ) );
    string contentType = "text/plain";
    string filename( "name.txt" );
    libcmis::DocumentPtr created = parent->createDocument( props, os, contentType, filename );

    // Check that something came back
    CPPUNIT_ASSERT_MESSAGE( "Change token shouldn't be empty: object should have been refreshed",
            !created->getChangeToken( ).empty() );

    // Check that the content is properly set
    shared_ptr< istream >  is = created->getContentStream( );
    stringstream buf;
    buf << is->rdbuf();
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong content set", contentStr, buf.str( ) );
}

void AtomTest::deleteDocumentTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );

    string id( "130" );
    libcmis::ObjectPtr object = session.getObject( id );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    CPPUNIT_ASSERT_MESSAGE( "Document to remove is missing", document != NULL );

    document->remove( );

    try
    {
        libcmis::ObjectPtr newObject = session.getObject( id );
        CPPUNIT_FAIL( "Should be removed, exception should have been thrown" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", string( "No such node: " + id ) , string( e.what() ) );
    }
}

void AtomTest::deleteTreeTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );

    string id( "117" );
    libcmis::ObjectPtr object = session.getObject( id );
    libcmis::Folder* folder = dynamic_cast< libcmis::Folder* >( object.get() );
    CPPUNIT_ASSERT_MESSAGE( "Document to remove is missing", folder != NULL );

    folder->removeTree( );

    try
    {
        libcmis::ObjectPtr newObject = session.getObject( id );
        CPPUNIT_FAIL( "Should be removed, exception should have been thrown" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", string( "No such node: " + id ) , string( e.what() ) );
    }
}

void AtomTest::checkOutTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );

    // First create a document of type VersionableType
    libcmis::DocumentPtr doc = test::createVersionableDocument( &session, "checkOutTest" );
    CPPUNIT_ASSERT_MESSAGE( "Failed to create versionable document", doc.get() != NULL );

    libcmis::DocumentPtr pwc = doc->checkOut( );
    
    CPPUNIT_ASSERT_MESSAGE( "Missing returned Private Working Copy", pwc.get( ) != NULL );

    map< string, libcmis::PropertyPtr >::iterator it = pwc->getProperties( ).find( string( "cmis:isVersionSeriesCheckedOut" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:isVersionSeriesCheckedOut property is missing", it != pwc->getProperties( ).end( ) );
    vector< bool > values = it->second->getBools( );
    CPPUNIT_ASSERT_MESSAGE( "cmis:isVersionSeriesCheckedOut isn't true", values.front( ) );
}

void AtomTest::cancelCheckOutTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );

    // First create a versionable document and check it out
    libcmis::DocumentPtr doc = test::createVersionableDocument( &session, "cancelCheckOutTest" );
    libcmis::DocumentPtr pwc = doc->checkOut( );

    CPPUNIT_ASSERT_MESSAGE( "Failed to create Private Working Copy document", pwc.get() != NULL );

    string id = pwc->getId( );
    pwc->cancelCheckout( );

    // Check that the PWC doesn't exist any more
    try
    {
        session.getObject( id );
        CPPUNIT_FAIL( "Private Working Copy object should no longer exist" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", string( "No such node: " ) + id, string( e.what() ) );
    }
}

void AtomTest::checkInTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );

    // First create a versionable document and check it out
    libcmis::DocumentPtr doc = test::createVersionableDocument( &session, "checkInTest" );
    libcmis::DocumentPtr pwc = doc->checkOut( );

    CPPUNIT_ASSERT_MESSAGE( "Failed to create Private Working Copy document", pwc.get() != NULL );

    // Do the checkin
    bool isMajor = true;
    string comment( "Some check-in comment" );
    map< string, libcmis::PropertyPtr > properties;
    string newContent = "Some New content to check in";
    boost::shared_ptr< ostream > stream ( new stringstream( newContent ) );
    pwc->checkIn( isMajor, comment, properties, stream, "text/plain", "filename.txt" );

    map< string, libcmis::PropertyPtr > actualProperties = pwc->getProperties( );

    {
        map< string, libcmis::PropertyPtr >::iterator it = actualProperties.find( "cmis:isLatestVersion" );
        CPPUNIT_ASSERT_MESSAGE( "cmis:isLatestVersion isn't true", it->second->getBools().front( ) );
    }
    
    {
        map< string, libcmis::PropertyPtr >::iterator it = actualProperties.find( "cmis:isMajorVersion" );
        CPPUNIT_ASSERT_MESSAGE( "cmis:isMajorVersion isn't true", it->second->getBools().front( ) );
    }

    {
        map< string, libcmis::PropertyPtr >::iterator it = actualProperties.find( "cmis:checkinComment" );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "cmis:checkinComment doesn't match", comment, it->second->getStrings().front( ) );
    }
}

void AtomTest::getAllVersionsTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );

    // First create a versionable document and check it out
    libcmis::DocumentPtr doc = test::createVersionableDocument( &session, "getAllVersionsTest" );
    libcmis::DocumentPtr pwc = doc->checkOut( );

    CPPUNIT_ASSERT_MESSAGE( "Failed to create Private Working Copy document", pwc.get() != NULL );

    // Create a version
    bool isMajor = true;
    string comment( "Some check-in comment" );
    map< string, libcmis::PropertyPtr > properties;
    string newContent = "Some New content to check in";
    boost::shared_ptr< ostream > stream ( new stringstream( newContent ) );
    libcmis::DocumentPtr newVersion = pwc->checkIn( isMajor, comment, properties, stream, "text/plain", "filename.txt" );

    // Get all the versions (method to check)
    vector< libcmis::DocumentPtr > versions = newVersion->getAllVersions( ); 

    // Checks
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of versions", size_t( 2 ), versions.size( ) );
}

void AtomTest::moveTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );
    
    string id( "135" );
    libcmis::ObjectPtr object = session.getObject( id );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    CPPUNIT_ASSERT_MESSAGE( "Document to move is missing", document != NULL );

    libcmis::FolderPtr src = document->getParents( ).front( );
    libcmis::FolderPtr dest = session.getFolder( "101" );

    document->move( src, dest );

    vector< libcmis::FolderPtr > parents = document->getParents( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parents size", size_t( 1 ), parents.size( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent", string( "101" ), parents.front( )->getId( ) );
}

CPPUNIT_TEST_SUITE_REGISTRATION( AtomTest );