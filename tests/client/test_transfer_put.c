/*******************************************************************************
 * Copyright (C) 2004-2006 Intel Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  - Neither the name of Intel Corp. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL Intel Corp. OR THE CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/**
 * @author Anas Nashif
 */
#include "wsman_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "u/libu.h"
#include "wsman-xml-api.h"
#include "wsman-errors.h"
#include "wsman-soap.h"
#include "wsman-xml.h"
#include "wsman-xml-serializer.h"
#include "wsman-debug.h"

#include "wsman-client.h"
#include "wsman-client-transport.h"



int facility = LOG_DAEMON;
int errors = 0;
char *host = "langley.home.planux.com";



typedef struct {
	const char *server;
	int port;
	const char *path;
	const char *scheme;
	const char *username;
	const char *password;
} ServerData;

typedef struct {						
        /* Explanation of what you should see */
        const char *explanation;

        /* Resource UR to test against */
        const char *resource_uri;

		/* Selectors in the form of a URI query   key=value&key2=value2 */
		char *selectors;
		const char *properties;
		
        const char* xpath_expression;
        const char* expected_value;
				
        /* What the final status code should be. */
        unsigned int final_status;		
		
		
} TestData;


ServerData sd[] = {
	{"localhost", 8889, "/wsman", "http", "wsman", "secret"}
};

TestData tests[] = {
	{
	    "Transfer Put without any selectors, Check Fault Value", 
	    "http://schemas.dmtf.org/wbem/wscim/1/cim-schema/2/CIM_ComputerSystem", 
	    NULL,
	    NULL,
		"/s:Envelope/s:Body/s:Fault/s:Code/s:Subcode/s:Value",
		"wsman:InvalidSelectors",	    
	    500
	},
    {
        "Transfer Put with wrong destination, Check Fault Value", 
        "http://schemas.dmtf.org/wbem/wscim/1/cim-schema/2/CIM_ComputerSystemx", 
        "CreationClassName=OpenWBEM_UnitaryComputerSystem&Name=%s",
        NULL,
        "/s:Envelope/s:Body/s:Fault/s:Code/s:Subcode/s:Value",
        "wsa:DestinationUnreachable",
        500
    },
    {
        "Transfer Put with wrong selectors, Check Fault Value", 
        "http://schemas.dmtf.org/wbem/wscim/1/cim-schema/2/CIM_ComputerSystem", 
        "CreationClassName=OpenWBEM_UnitaryComputerSystemx&Name=%s",
        NULL,
        "/s:Envelope/s:Body/s:Fault/s:Code/s:Subcode/s:Value",
        "wsman:InvalidRepresentation",
        500
    },

    {
        "Transfer Put with wrong content, Check Fault Value", 
        "http://schemas.dmtf.org/wbem/wscim/1/cim-schema/2/CIM_ComputerSystem", 
        "CreationClassName=OpenWBEM_UnitaryComputerSystem&Name=%s",
        NULL,
        "/s:Envelope/s:Body/s:Fault/s:Code/s:Subcode/s:Value",
        "wsman:InvalidRepresentation",
        500
    },
};

int ntests = sizeof (tests) / sizeof (tests[0]);

#if 0
static void wsman_output(WsXmlDocH doc)
{
	ws_xml_dump_node_tree(stdout, ws_xml_get_doc_root(doc));
	return;
}

#endif

int main(int argc, char** argv)
{
	int i;
	WsManClient *cl;
	WsXmlDocH doc;
	actionOptions options;
		
    if (getenv("OPENWSMAN_TEST_HOST")) {
        host = getenv("OPENWSMAN_TEST_HOST");
    }
//  wsman_debug_set_level(6);
	
	wsman_client_transport_init(NULL);
	
		
	for (i = 0; i < ntests; i++) 
	{
		printf ("Test %3d: %s. ", i + 1, tests[i].explanation);
        if (tests[i].selectors) {
            tests[i].selectors = u_strdup_printf(tests[i].selectors, host, host, host);
        }
    	cl = wsman_create_client( sd[0].server,
    		sd[0].port,
    		sd[0].path,
    		sd[0].scheme,
    		sd[0].username,
    		sd[0].password);		
		initialize_action_options(&options);
		
		if (tests[i].selectors != NULL)
			wsman_add_selectors_from_query_string(&options, tests[i].selectors);
		if (tests[i].properties != NULL)
			wsman_add_properties_from_query_string(&options, tests[i].properties);		
		 
		doc = ws_transfer_get_and_put(cl, (char *)tests[i].resource_uri, options);
        if (!doc) {
                printf("\t\t\033[22;31mUNRESOLVED\033[m\n");
                goto CONTINUE;
        }
       if (tests[i].final_status != wsman_get_client_response_code(cl)) {
            printf("Status = %ld \t\t\033[22;31mFAILED\033[m\n",
                                wsman_get_client_response_code(cl));
            goto CONTINUE;
        }
        if ((char *)tests[i].expected_value != NULL) {
            char *xp = ws_xml_get_xpath_value(doc, (char *)tests[i].xpath_expression);
            if (xp) {
                if (strcmp(xp,(char *)tests[i].expected_value ) == 0)
                    printf("\t\t\033[22;32mPASSED\033[m\n");
                else
                    printf("%s = %s\t\033[22;31mFAILED\033[m\n",
                        (char *)tests[i].xpath_expression, xp);
                u_free(xp);
            } else {
                printf(" No %s\t\033[22;31mFAILED\033[m\n",
                        (char *)tests[i].xpath_expression);
                ws_xml_dump_node_tree(stdout, ws_xml_get_doc_root(doc));
            }
        } else {
            printf("\t\t\033[22;32mPASSED\033[m\n");
        }
        ws_xml_destroy_doc(doc);
CONTINUE:
		destroy_action_options(&options);
        wsman_release_client(cl);
	}
	
	return 0;
}


