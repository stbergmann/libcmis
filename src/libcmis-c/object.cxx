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

#include <libcmis/object.hxx>

#include "internals.hxx"
#include "object.h"


void libcmis_object_free( libcmis_ObjectPtr object )
{
    delete object;
}


const char* libcmis_object_getId( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->getId( ).c_str( );
    else
        return NULL;
}


const char* libcmis_object_getName( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->getName( ).c_str( );
    else
        return NULL;
}

const char** libcmis_object_getPaths( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
    {
        std::vector< std::string > paths = object->handle->getPaths( );
        const char** c_paths = new const char*[ paths.size( ) ];
        for ( size_t i = 0; i < paths.size( ); ++i )
        {
            c_paths[i] = paths[i].c_str( );
        }
        return c_paths;
    }
    else
        return NULL;
}

const char* libcmis_object_getBaseType( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->getBaseType( ).c_str( );
    else
        return NULL;
}


const char* libcmis_object_getType( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->getType( ).c_str( );
    else
        return NULL;
}


const char* libcmis_object_getCreatedBy( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->getCreatedBy( ).c_str( );
    else
        return NULL;
}


time_t libcmis_object_getCreationDate( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
    {
        tm time = boost::posix_time::to_tm( object->handle->getCreationDate( ) );
        return mktime( &time );
    }
    else
        return 0;
}


const char* libcmis_object_getLastModifiedBy( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->getLastModifiedBy( ).c_str( );
    else
        return NULL;
}


time_t libcmis_object_getLastModificationDate( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
    {
        tm time = boost::posix_time::to_tm( object->handle->getLastModificationDate( ) );
        return mktime( &time );
    }
    else
        return 0;
}


const char* libcmis_object_getChangeToken( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->getChangeToken( ).c_str( );
    else
        return NULL;
}


bool libcmis_object_isImmutable( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->isImmutable( );
    else
        return true;
}


/* TODO libcmis_object_getProperties */


void libcmis_object_updateProperties( libcmis_ObjectPtr object, libcmis_ErrorPtr error )
{
    if ( object != NULL && object->handle != NULL )
    {
        try
        {
            object->handle->updateProperties( );
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }
}


/* TODO libcmis_object_getTypeDescription */


/* TODO libcmis_object_getAllowableActions */


void libcmis_object_refresh( libcmis_ObjectPtr object, libcmis_ErrorPtr error )
{
    if ( object != NULL && object->handle != NULL )
    {
        try
        {
            object->handle->refresh( );
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }
}


time_t libcmis_object_getRefreshTimestamp( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->getRefreshTimestamp( );
    else
        return 0;
}

void libcmis_object_remove( libcmis_ObjectPtr object, bool allVersions, libcmis_ErrorPtr error )
{
    if ( object != NULL && object->handle != NULL )
    {
        try
        {
            object->handle->remove( allVersions );
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }
}

const char* libcmis_object_toString( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->toString( ).c_str( );
    else
        return NULL;
}