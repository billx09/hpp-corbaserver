// Copyright (C) 2009, 2010 by Florent Lamiraux, Thomas Moulard, JRL.
//
// This file is part of the hpp-corbaserver.
//
// This software is provided "as is" without warranty of any kind,
// either expressed or implied, including but not limited to the
// implied warranties of fitness for a particular purpose.
//
// See the COPYING file for more information.

#include <iostream>

#include <hpp/util/debug.hh>

#include "server-private.hh"

namespace hpp
{
  namespace corbaServer
  {
    namespace impl
    {
      using CORBA::Exception;
      using CORBA::ORB_init;
      using CORBA::Object_ptr;
      using CORBA::Object_var;
      using CORBA::PolicyList;
      using CORBA::SystemException;
      using CORBA::COMM_FAILURE;
      using omniORB::fatalException;

      typedef CORBA::ORB::InvalidName InvalidName;

      Server::~Server ()
      {
      }

      void
      Server::createAndActivateServers (corbaServer::Server* inServer)
      {
        Servers servers;
        std::size_t idx = servers_.size();

	servers.robotServant_ = new Robot (inServer, idx);
	servers.obstacleServant_ = new Obstacle (inServer, idx);
	servers.problemServant_ = new Problem (inServer, idx);
	servers.robotServantid_ = poa_->activate_object(servers.robotServant_);
	servers.obstacleServantid_ = poa_->activate_object(servers.obstacleServant_);
        servers.problemServantid_ = poa_->activate_object(servers.problemServant_);
        servers.selectedProblemSolver_ = "default";

        servers_.push_back (servers);
      }

      void Server::deactivateAndDestroyServers()
      {
        orb_->destroy();
      }


      void Server::createHppContext (const std::string& mainContextId)
      {
	CosNaming::NamingContext_var rootContext;
	Object_var localObj;
	CosNaming::Name contextName;

	// Obtain a reference to the root context of the Name service:
	localObj = orb_->resolve_initial_references("NameService");
	try {
	  // Narrow the reference returned.
	  rootContext = CosNaming::NamingContext::_narrow(localObj);
	  if( is_nil(rootContext) ) {
	    std::string msg ("Failed to narrow the root naming context.");
	    hppDout (error, msg.c_str ());
	    throw std::runtime_error (msg.c_str ());
	  }
	}
	catch(InvalidName& ex) {
	  // This should not happen!
	  std::string msg ("Service required is invalid [does not exist].");
	  hppDout (error, msg.c_str ());
	  throw std::runtime_error (msg.c_str ());
	}
	// Bind a context called "hpp" to the root context:
        contextName.length(1);
        contextName[0].id   = (const char*) mainContextId.c_str(); // string copied
	contextName[0].kind = (const char*) "corbaserver"; // string copied
	// Note on kind: The kind field is used to indicate the type
	// of the object. This is to avoid conventions such as that used
	// by files (name.type -- e.g. hpp.ps = postscript etc.)

	try {
	  // Bind the context to root.
	  hppContext_ = rootContext->bind_new_context(contextName);
	}
	catch(CosNaming::NamingContext::AlreadyBound& ex) {
	  // If the context already exists, this exception will be raised.
	  // In this case, just resolve the name and assign hppContext
	  // to the object returned:
	  Object_var localObj;
	  localObj = rootContext->resolve(contextName);
	  hppContext_ = CosNaming::NamingContext::_narrow(localObj);
	  if( is_nil(hppContext_) ) {
	    std::string msg ("Failed to narrow naming context.");
	    hppDout (error, msg.c_str ());
	    throw std::runtime_error (msg.c_str ());
	  }
	}
      }

      void Server::bindObjectToName(Object_ptr objref,
				    CosNaming::Name objectName)
      {
	try {
	  hppContext_->bind(objectName, objref);
	}
	catch(CosNaming::NamingContext::AlreadyBound& ex) {
	  hppContext_->rebind(objectName, objref);
	}
	// Note: Using rebind() will overwrite any Object previously bound
	//       to /hpp/RobotConfig with localObj.
	//       Alternatively, bind() can be used, which will raise a
	//       CosNaming::NamingContext::AlreadyBound exception if the name
	//       supplied is already bound to an object.

	// Amendment: When using OrbixNames, it is necessary to first
	// try bind and then rebind, as rebind on it's own will throw
	// a NotFoundexception if the Name has not already been
	// bound. [This is incorrect behaviour - it should just bind].
      }
    } // end of namespace impl.
  } // end of namespace corbaServer.
} // end of namespace hpp.
