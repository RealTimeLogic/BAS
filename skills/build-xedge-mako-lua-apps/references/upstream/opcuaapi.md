

---

# Source: opcua/about.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="#" class="current reference internal">About opcua-lua</a>
  - <a href="#why-lua" class="reference internal">Why Lua</a>
  - <a href="#where-it-fits" class="reference internal">Where it fits</a>
  - <a href="#what-to-read-next" class="reference internal">What to read next</a>
- <a href="opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="model/index.html" class="reference internal">Address Space API</a>
- <a href="client/index.html" class="reference internal">Client</a>
- <a href="server/index.html" class="reference internal">Server</a>
- <a href="pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="integrations/index.html" class="reference internal">Examples</a>
- <a href="logging.html" class="reference internal">Logging subsystem</a>
- <a href="secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="types.html" class="reference internal">Data Types</a>
- <a href="interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="index.html" class="icon icon-home" aria-label="Home"></a>
- About opcua-lua
- <a href="_sources/about.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="about-opcua-lua" class="section">

# About opcua-lua<a href="#about-opcua-lua" class="headerlink" title="Link to this heading"></a>

<span class="pre">`opcua-lua`</span> is Real Time Logic’s OPC UA stack for Lua applications. It is designed for systems where OPC UA connectivity must live close to the device, gateway, web server, or embedded application instead of being isolated in a large external service.

The stack supports the standard OPC UA Client/Server model and OPC UA PubSub over MQTT. This means the same Lua application can expose an address space, connect to another OPC UA server, publish selected values over MQTT, or consume PubSub messages.

<div id="why-lua" class="section">

## Why Lua<a href="#why-lua" class="headerlink" title="Link to this heading"></a>

Lua is small, embeddable, and fast to iterate with. In Real Time Logic products, Lua also runs inside the Barracuda App Server environment used by <a href="https://makoserver.net/" class="reference external">Mako Server</a>, <a href="https://realtimelogic.com/products/xedge/" class="reference external">Xedge</a>, and <a href="https://realtimelogic.com/downloads/bas/ESP32/?bas=" class="reference external">Xedge32</a>. This makes it possible to combine industrial automation, device logic, web APIs, dashboards, and deployment scripts in one runtime.

The main benefits are:

**Lightweight embedded design**  
OPC UA functionality can run in constrained environments and edge devices without the footprint of many native desktop/server stacks.

**Rapid application development**  
Address-space logic, data sources, authentication callbacks, and PubSub workflows can be scripted and changed quickly.

**No native thread dependency**  
The client can use non-blocking cosocket mode, allowing asynchronous I/O without one operating-system thread per connection.

**Web and device integration**  
The same runtime can expose OPC UA data, serve web applications, and connect browser dashboards or cloud services.

**Client/Server and PubSub support**  
Applications can use browse/read/write/method calls when they need the OPC UA information model, and MQTT PubSub when they need message-based distribution.

</div>

<div id="where-it-fits" class="section">

## Where it fits<a href="#where-it-fits" class="headerlink" title="Link to this heading"></a>

<span class="pre">`opcua-lua`</span> is a good fit when the OPC UA layer needs to be part of an application instead of a separate industrial gateway process.

Common use cases include:

- Embedded devices that expose sensor or control data through OPC UA.

- Edge gateways that translate local device data into OPC UA and MQTT.

- <a href="https://makoserver.net/" class="reference external">Mako Server</a> applications that combine OPC UA with web APIs or browser-based operator views.

- <a href="https://realtimelogic.com/downloads/bas/ESP32/?bas=" class="reference external">Xedge32</a> applications that expose ESP32 device state through OPC UA.

- Prototypes where the information model, authentication, and data source logic are still changing.

</div>

<div id="what-to-read-next" class="section">

## What to read next<a href="#what-to-read-next" class="headerlink" title="Link to this heading"></a>

Start with <a href="opcua_overview.html" class="reference internal"><span class="doc">OPC UA overview</span></a> if OPC UA itself is new to you. Read <a href="pubsub/client_server_vs_pubsub.html" class="reference internal"><span class="doc">Client/Server vs PubSub over MQTT</span></a> if you need to decide between a browseable OPC UA server and MQTT PubSub messages.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="index.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="OPC UA Client and Server"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="opcua_overview.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="OPC UA overview">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/application_certificate.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="about.html" class="reference internal">About opcua-lua</a>
- <a href="opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="model/index.html" class="reference internal">Address Space API</a>
- <a href="client/index.html" class="reference internal">Client</a>
- <a href="server/index.html" class="reference internal">Server</a>
- <a href="pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="integrations/index.html" class="reference internal">Examples</a>
- <a href="logging.html" class="reference internal">Logging subsystem</a>
- <a href="secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="#" class="current reference internal">Application Certificate</a>
  - <a href="#creating-certificate-with-xlua" class="reference internal">Creating certificate with xlua</a>
  - <a href="#creating-openssl-certificate" class="reference internal">Creating OpenSSL certificate</a>
- <a href="types.html" class="reference internal">Data Types</a>
- <a href="interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="index.html" class="icon icon-home" aria-label="Home"></a>
- Application Certificate
- <a href="_sources/application_certificate.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="application-certificate" class="section">

# Application Certificate<a href="#application-certificate" class="headerlink" title="Link to this heading"></a>

OPC UA applications use certificates to store the public keys needed for asymmetric cryptography operations. All security protocols use X.509 v3 certificates (see X.509 v3) encoded using the DER format (see X690). The server certificate and client certificate are used in the abstract OpenSecureChannel service.

<div class="admonition note">

Note

See the tutorial <a href="https://realtimelogic.com/articles/Certificate-Management-for-Embedded-Systems" class="reference external">An Introduction to Public Key Infrastructure</a> if you are new to X.509 certificate management.

</div>

The OPC UA Application Instance certificate **must include a URI** in the SubjectAltNames along with a hostname. The Application URI is used during the opening of a secure channel and checked to ensure it is present in the application certificate. Without this, the certificate will be rejected.

<div id="creating-certificate-with-xlua" class="section">

## Creating certificate with xlua<a href="#creating-certificate-with-xlua" class="headerlink" title="Link to this heading"></a>

The following shows how to create a certificate programmatically:

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")
    local hostname = "localhost"
    local applicationUri = "urn:localhost:RealTimeLogic"

    print("generating private key")
    local basic128rsa15Cert, basic128rsa15Key = ua.Init.genServerCertificate(hostname, applicationUri)

    print(basic128rsa15Key)
    print(basic128rsa15Cert)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/create_certificate_basic128rsa15.lua" class="reference external">Full source</a>

The following command will execute the script that will print out the private key and self-signed certificate:

<div class="highlight-sh notranslate">

<div class="highlight">

    xlua create_certificate_basic128rsa15.lua

</div>

</div>

</div>

<div id="creating-openssl-certificate" class="section">

## Creating OpenSSL certificate<a href="#creating-openssl-certificate" class="headerlink" title="Link to this heading"></a>

To create an SSL certificate, you need to create a configuration file. The uniqueness of OPC UA certificate creation is that SubjectAltNames contains a URI extension specifying the Application URI, which clients use to validate the peer application.

<div class="highlight-ini notranslate">

<div class="highlight">

    [req]
    default_bits = 2048
    prompt = no
    default_md = sha256
    encrypt_key = no
    x509_extensions = v3_req
    distinguished_name = dn

    [dn]
    C = US
    ST = Washington
    L = NY
    O = RealTimeLogic
    emailAddress = example@email.com
    CN = localhost

    [v3_req]
    subjectAltName = URI:urn:localhost:RealTimeLogic

    [alt_names]
    DNS.1 = localhost

</div>

</div>

The following OpenSSL command will generate an OPCUA Application Certificate that can be used with the basic128rsa15 security policy.

<div class="highlight-sh notranslate">

<div class="highlight">

    openssl req -config basic128rsa15.conf -newkey rsa -x509 -days 365 -keyout basic128rsa15_server.key -out basic128rsa15_client.pem

</div>

</div>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="secure_policy.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Secure Policy"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="types.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Data Types">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/client/adding_nodes.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="index.html" class="reference internal">Client</a>
  - <a href="getting_started.html" class="reference internal">Getting Started</a>
  - <a href="authentication.html" class="reference internal">Client Authentication</a>
  - <a href="browsing.html" class="reference internal">Address Space Browsing</a>
  - <a href="read_write_data.html" class="reference internal">Reading and Writing Data</a>
  - <a href="#" class="current reference internal">Adding Nodes</a>
    - <a href="#node-attributes" class="reference internal">Node attributes</a>
    - <a href="#folder-and-variable-nodes" class="reference internal">Folder and Variable nodes</a>
      - <a href="#ua.newFolderParams" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newFolderParams()</code></span></a>
      - <a href="#ua.newVariableParams" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newVariableParams()</code></span></a>
    - <a href="#adding-nodes-example" class="reference internal">Adding nodes example</a>
    - <a href="#new-node-common-attributes" class="reference internal">New node common attributes</a>
    - <a href="#object-nodeattributes" class="reference internal">Object NodeAttributes</a>
      - <a href="#object-nodeattributes-example" class="reference internal">Object NodeAttributes example</a>
  - <a href="http_client.html" class="reference internal">HTTP Client</a>
  - <a href="init.html" class="reference internal">OPC UA Project Code Generator</a>
  - <a href="client_api.html" class="reference internal">Client API</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Client](index.html)
- Adding Nodes
- <a href="../_sources/client/adding_nodes.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="adding-nodes" class="section">

# Adding Nodes<a href="#adding-nodes" class="headerlink" title="Link to this heading"></a>

The OPC UA address space consists of a set of nodes interconnected by various types of references. There are two main categories of references: hierarchical and non-hierarchical.

Hierarchical references are used to construct a tree structure of nodes, which is commonly visible in different UI clients.

Non-hierarchical references have a special significance and are used to describe the relationships between nodes. For instance, the HasTypeDefinition reference type is used to define a type with a predefined set of fields.

<div id="node-attributes" class="section">

## Node attributes<a href="#node-attributes" class="headerlink" title="Link to this heading"></a>

Every node in OPC UA can be one of the following classes:

- Variable

- VariableType

- Object

- ObjectType

- Method

- View

- DataType

- ReferenceType

Each node has a set of attributes that describe it. The full list of attributes is detailed in the <a href="https://reference.opcfoundation.org/Core/Part3/v105/docs/5" class="reference external">OPC-UA specification</a>.

</div>

<div id="folder-and-variable-nodes" class="section">

## Folder and Variable nodes<a href="#folder-and-variable-nodes" class="headerlink" title="Link to this heading"></a>

The most basic nodes in the address space are Folder and Variable nodes. Folder nodes are used to organize the address space into a tree structure, while Variable nodes are used to store data values.

The SDK provides helper functions to simplify the process of adding Folder and Variable nodes to the address space:

<span class="sig-prename descclassname"><span class="pre">ua.</span></span><span class="sig-name descname"><span class="pre">newFolderParams</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">parentNodeId</span></span>*, *<span class="n"><span class="pre">nodeName</span></span>*, *<span class="n"><span class="pre">requestedNodeId</span></span>*<span class="sig-paren">)</span><a href="#ua.newFolderParams" class="headerlink" title="Link to this definition"></a>  
Function creates a table with parameters for adding a new folder node.

<!-- -->

<span class="sig-prename descclassname"><span class="pre">ua.</span></span><span class="sig-name descname"><span class="pre">newVariableParams</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">parentNodeId</span></span>*, *<span class="n"><span class="pre">nodeName</span></span>*, *<span class="n"><span class="pre">defaultValue</span></span>*, *<span class="n"><span class="pre">requestedNodeId</span></span>*<span class="sig-paren">)</span><a href="#ua.newVariableParams" class="headerlink" title="Link to this definition"></a>  
Function creates a table with parameters for adding a new variable node.

<!-- -->

parentNodeId<span class="colon">:</span>  
( <a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> ) Parent node identifier.

nodeName<span class="colon">:</span>  
(string) The internal name for the node. This name is used in the TranslateBrowsePathsToNodeIds service to resolve NodeId by the path from some nodes.

defaultValue<span class="colon">:</span>  
(<a href="../types.html#data-value-type" class="reference internal"><span class="std std-ref">DataValue</span></a>) The default value for the variable.

requestedNodeId<span class="colon">:</span>  
( <a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> ) NodeID for the new node. The identifier will be automatically assigned by the server if NodeId equals ua.NodeId.Null.

</div>

<div id="adding-nodes-example" class="section">

## Adding nodes example<a href="#adding-nodes-example" class="headerlink" title="Link to this heading"></a>

A Variable in OPC-UA is a node in the address space with a set of predefined attributes:

> <div>
>
> <div class="highlight-lua notranslate">
>
> <div class="highlight">
>
>       applicationName = 'RealTimeLogic example',
>       applicationUri = "urn:opcua-lua:example",
>       productUri = "urn:opcua-lua:example",
>       securePolicies = {
>         { -- #1
>           securityPolicyUri = ua.SecurityPolicy.None
>         }
>       },
>     }
>
>     local client = ua.newClient(config)
>
>     local resp, err
>
>     -- Connecting to OPCUA server
>     trace("connecting to server")
>     local endpointUrl = "opc.tcp://localhost:4841"
>     err = client:connect(endpointUrl)
>     if err ~= nil then
>       return
>     end
>
>     -- Open secure channel with timeout 120 seconds
>     resp, err = client:openSecureChannel(120000, ua.SecurityPolicy.None, ua.MessageSecurityMode.None)
>     if err ~= nil then
>       return
>     end
>
>     resp, err = client:createSession("test_session", 3600000)
>     if err ~= nil then
>       trace("Creating session failed: "..err)
>       return
>     end
>
> </div>
>
> </div>
>
> <a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_add_nodes.lua" class="reference external">Full source</a>
>
> </div>

</div>

<div id="new-node-common-attributes" class="section">

## New node common attributes<a href="#new-node-common-attributes" class="headerlink" title="Link to this heading"></a>

To add new object into address space you need to pass a table with the following parameters:

> <div>
>
> **ParentNodeId** ( <a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> )  
> Parent node identifier.
>
> **ReferenceTypeId** (<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>)  
> Parent’s reference type identifier.
>
> **RequestedNewNodeId** (<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>)  
> NodeID for the new node. The identifier will be automatically assigned by the server if NodeId equals ua.NodeId.Null.
>
> **BrowseName** (<a href="../types.html#qualified-name-type" class="reference internal"><span class="std std-ref">QualifiedName</span></a>)  
> The internal non-localizable name for the node. This name is used in the TranslateBrowsePathsToNodeIds service to resolve NodeId by the path from some nodes.
>
> **NodeClass** (uint8)  
> Node class equal to ua.NodeClass.Object,
>
> **TypeDefinition** (<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>)  
> Node ID of the object type definition. According to this, an underlying node hierarchy will be created.
>
> **NodeAttributes** (<a href="../types.html#extension-object-type" class="reference internal"><span class="std std-ref">ExtensionObject</span></a>)  
> An Extension object table with attributes specific to the object node. The following sections describe the full set of attributes for different node classes.
>
> </div>

</div>

<div id="object-nodeattributes" class="section">


## Object NodeAttributes<a href="#object-nodeattributes" class="headerlink" title="Link to this heading"></a>

**TypeId** (<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>)  
Node ID of Object attributes “i=354”

**Body** (<a href="../types.html#extension-object-type" class="reference internal"><span class="std std-ref">ExtensionObject</span></a>)  
A body of extension object attributes:

**SpecifiedAttributes** = ua.ObjectAttributesMask

**DisplayName** (<a href="../types.html#localized-text-type" class="reference internal"><span class="std std-ref">LocalizedText</span></a>)  
Clients use this attribute if they want to show the name of the node to the user. Can be localized.

**Description** (<a href="../types.html#localized-text-type" class="reference internal"><span class="std std-ref">LocalizedText</span></a>)  
The optional description attribute explains the purpose of the node using localized text.

**WriteMask** (UInt32)  
Bit mask. Makes it possible for a client to write the Attributes of the Node.

**UserWriteMask** (UInt32)  
Bit mask. Makes it possible for a client to write the Attributes of the Node.

**EventNotifier** (Byte)  
Event notifier.

<div id="object-nodeattributes-example" class="section">

### Object NodeAttributes example<a href="#object-nodeattributes-example" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    local folderParams = { -- #1
      ParentNodeId = ObjectsFolder,
      ReferenceTypeId = Organizes,
      RequestedNewNodeId = "i=1000",
      BrowseName = {Name="TestFolder", ns=0},
      NodeClass = ua.NodeClass.Object,
      TypeDefinition = FolderType,
      NodeAttributes = {
        TypeId = "i=354",
        Body = {
          SpecifiedAttributes = ua.ObjectAttributesMask,
          DisplayName = {Text="DisplayName"},
          Description = {Text="Description"},
          WriteMask = 0,
          UserWriteMask = 0,
          EventNotifier = 0,
        }
      }
    }

</div>

</div>

</div>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="read_write_data.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Reading and Writing Data"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="http_client.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="HTTP Client">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/client/authentication.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="index.html" class="reference internal">Client</a>
  - <a href="getting_started.html" class="reference internal">Getting Started</a>
  - <a href="#" class="current reference internal">Client Authentication</a>
    - <a href="#working-with-client-sessions" class="reference internal">Working with client sessions</a>
    - <a href="#anonymous" class="reference internal">Anonymous</a>
    - <a href="#user-name" class="reference internal">User Name</a>
    - <a href="#x509-certificate" class="reference internal">x509 certificate</a>
    - <a href="#jwt-oauth2-azure" class="reference internal">JWT, OAuth2, Azure</a>
  - <a href="browsing.html" class="reference internal">Address Space Browsing</a>
  - <a href="read_write_data.html" class="reference internal">Reading and Writing Data</a>
  - <a href="adding_nodes.html" class="reference internal">Adding Nodes</a>
  - <a href="http_client.html" class="reference internal">HTTP Client</a>
  - <a href="init.html" class="reference internal">OPC UA Project Code Generator</a>
  - <a href="client_api.html" class="reference internal">Client API</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Client](index.html)
- Client Authentication
- <a href="../_sources/client/authentication.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="client-authentication" class="section">

# Client Authentication<a href="#client-authentication" class="headerlink" title="Link to this heading"></a>

<div id="working-with-client-sessions" class="section">

## Working with client sessions<a href="#working-with-client-sessions" class="headerlink" title="Link to this heading"></a>

After opening a secure channel, the client needs to create a session. The client will receive a set of endpoints and a list of supported user identity token policies in response.

<div class="highlight-lua notranslate">

<div class="highlight">

    local session, err = client:createSession("test_session", 3600000)
    if err ~= nil then
      error("Creating session failed: "..err)
    end

    local tokenPolicy
    for _, endpoint in ipairs(session.ServerEndpoints) do
      for _, policy in ipairs(endpoint.UserIdentityTokens) do
        if policy.TokenType == ua.UserTokenType.UserName and
          (policy.SecurityPolicyUri == nil or policy.SecurityPolicyUri == ua.SecurityPolicy.None)
        then
          tokenPolicy = policy
          goto found
        end
      end
    end

    ::found::
    if not tokenPolicy then
      error("cannot find endpoint with username token.")
    end

    local userName = "admin"
    local password = "12345"
    local resp, err = client:activateSession(tokenPolicy.PolicyId, userName, password)
    if err ~= nil then
      error("Activating session failed: "..err)
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_auth_username.lua" class="reference external">Full source</a>

Each token policy contains several fields. These fields are common to all policy types:

| Field                                        | Description                                                                                             |
|----------------------------------------------|---------------------------------------------------------------------------------------------------------|
| <span class="pre">`PolicyId`</span>          | Server-assigned policy identifier. The client sends it back so the server can interpret the user token. |
| <span class="pre">`TokenType`</span>         | Token type: Anonymous, UserName, Certificate, or IssuedToken.                                           |
| <span class="pre">`SecurityPolicyUri`</span> | Security policy URI used to secure the token.                                                           |

Additional fields are specific to each token type and will be covered in the following sections.

</div>

<div id="anonymous" class="section">

## Anonymous<a href="#anonymous" class="headerlink" title="Link to this heading"></a>

An anonymous token basically means the absence of authentication, which means anyone can connect to and work with the server. This type of authentication is usually enabled for testing purposes only.

You can pass the identifier of the anonymous policy, which is taken from the response of CreateSession. No additional parameters are required for the anonymous token.

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Create session with name "test_session" and with life time
    -- 1 hour (3600000 ms)
    local session, err = client:createSession("test_session", 3600000)
    local tokenPolicy
    for _, endpoint in ipairs(session.ServerEndpoints) do
      for _, policy in ipairs(endpoint.UserIdentityTokens) do
        if  policy.TokenType == ua.UserTokenType.Anonymous
        then
          tokenPolicy = policy
          goto found
        end
      end
    end

    ::found::
    if not tokenPolicy then
      error("cannot find endpoint with anonymous token.")
    end
    -- Passing id of anonymous token policy.
    local resp, err = client:activateSession(tokenPolicy.PolicyId)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_auth_anonymous.lua" class="reference external">Full source</a>

</div>

<div id="user-name" class="section">

## User Name<a href="#user-name" class="headerlink" title="Link to this heading"></a>

Depending on the server configuration, a password can be sent encrypted or unencrypted, which depends on the token policy configuration on the server.

The Username token policy has these parameters:

| Field                                        | Description                                                                                                                                                                                                                            |
|----------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <span class="pre">`PolicyId`</span>          | Server-assigned policy identifier.                                                                                                                                                                                                     |
| <span class="pre">`TokenType`</span>         | <span class="pre">`ua.UserTokenPolicy.UserName`</span>.                                                                                                                                                                                |
| <span class="pre">`SecurityPolicyUri`</span> | Security policy used to encrypt the password. If absent or set to <span class="pre">`None`</span>, the password is not encrypted by the token policy. If the secure channel also does not encrypt packets, the password is sent as-is. |

The following example demonstrates how to authenticate with a server using a username and password:

<div class="highlight-lua notranslate">

<div class="highlight">

    local session, err = client:createSession("test_session", 3600000)
    if err ~= nil then
      error("Creating session failed: "..err)
    end

    local tokenPolicy
    for _, endpoint in ipairs(session.ServerEndpoints) do
      for _, policy in ipairs(endpoint.UserIdentityTokens) do
        if policy.TokenType == ua.UserTokenType.UserName and
          (policy.SecurityPolicyUri == nil or policy.SecurityPolicyUri == ua.SecurityPolicy.None)
        then
          tokenPolicy = policy
          goto found
        end
      end
    end

    ::found::
    if not tokenPolicy then
      error("cannot find endpoint with username token.")
    end

    local userName = "admin"
    local password = "12345"
    local resp, err = client:activateSession(tokenPolicy.PolicyId, userName, password)
    if err ~= nil then
      error("Activating session failed: "..err)
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_auth_username.lua" class="reference external">Full source</a>

</div>

<div id="x509-certificate" class="section">

## x509 certificate<a href="#x509-certificate" class="headerlink" title="Link to this heading"></a>

To authenticate with a certificate, you need to find the ID of the policy with the type ua.UserTokenPolicy.Certificate.

This policy has these parameters:

| Field                                        | Description                                                                                                                                                                                                                                             |
|----------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <span class="pre">`PolicyId`</span>          | Server-assigned policy identifier.                                                                                                                                                                                                                      |
| <span class="pre">`TokenType`</span>         | <span class="pre">`ua.UserTokenPolicy.Certificate`</span>.                                                                                                                                                                                              |
| <span class="pre">`SecurityPolicyUri`</span> | Security policy that controls whether the client sends a signature. The signature is calculated with the user’s private key and proves ownership of the certificate. If absent or set to <span class="pre">`None`</span>, only the certificate is sent. |

When activating a session with certificate-based authentication, the ActivateSession call should include these parameters:

| Field                                  | Description                                                            |
|----------------------------------------|------------------------------------------------------------------------|
| <span class="pre">`PolicyId`</span>    | Certificate token policy identifier.                                   |
| <span class="pre">`TokenType`</span>   | <span class="pre">`ua.UserTokenPolicy.Certificate`</span>.             |
| <span class="pre">`Certificate`</span> | User certificate sent to the server.                                   |
| <span class="pre">`PrivateKey`</span>  | Private key for the user certificate. Used to calculate the signature. |

ActivateSession accepts certificate and private key in PEM or DER format, or a path to files can be passed.

The following example shows how to authenticate using a certificate:

<div class="highlight-lua notranslate">

<div class="highlight">

      error("Opening secure channel failed: "..err)
    end

    local session, err = client:createSession("test_session", 3600000)
    local tokenPolicy
    for _, endpoint in ipairs(session.ServerEndpoints) do
      -- Select certificate token policy with security policy Basic128Rsa15
      for _, policy in ipairs(endpoint.UserIdentityTokens) do
        if  policy.TokenType == ua.UserTokenType.Certificate then
          tokenPolicy = policy
          goto found
        end
      end
    end

    ::found::
    if not tokenPolicy then
      error("cannot find endpoint with certificate token policy.")
    end

    local certificate = mako.cfgdir.."/../certs/client.pem"
    local privateKey = mako.cfgdir.."/../certs/client.key"
    local resp, err = client:activateSession(
      tokenPolicy.PolicyId, certificate, privateKey)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_auth_x509.lua" class="reference external">Full source</a>

</div>

<div id="jwt-oauth2-azure" class="section">

## JWT, OAuth2, Azure<a href="#jwt-oauth2-azure" class="headerlink" title="Link to this heading"></a>

OPC UA allows authentication using third-party tokens, which are also known as Issued Tokens.

To authenticate with such tokens, you need to search for the policy that uses the token type ua.UserTokenPolicy.IssuedToken. This type of token is commonly used for all third-party tokens. To distinguish between token formats, the policy includes an additional parameter called IssuedTokenType.

The IssuedToken policy has these parameters:

| Field                                        | Description                                                                                                                                                                                                                   |
|----------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <span class="pre">`PolicyId`</span>          | Issued-token policy identifier.                                                                                                                                                                                               |
| <span class="pre">`TokenType`</span>         | <span class="pre">`ua.UserTokenPolicy.IssuedToken`</span>.                                                                                                                                                                    |
| <span class="pre">`IssuedTokenType`</span>   | Token format: <span class="pre">`ua.IssuedTokenType.JWT`</span>, <span class="pre">`ua.IssuedTokenType.OAuth2`</span>, or <span class="pre">`ua.IssuedTokenType.Azure`</span>.                                                |
| <span class="pre">`IssuerEndpointUrl`</span> | URL of the identity server that issued the token.                                                                                                                                                                             |
| <span class="pre">`SecurityPolicyUri`</span> | Security policy used to encrypt the token. If absent or set to <span class="pre">`None`</span>, the token is not encrypted by the token policy. If the secure channel also does not encrypt packets, the token is sent as-is. |

The following example shows how to authenticate with issued token:

<div class="highlight-lua notranslate">

<div class="highlight">

      error("Opening secure channel failed: "..err)
    end

    local session, err = client:createSession("test_session", 3600000)
    local tokenPolicy
    for _, endpoint in ipairs(session.ServerEndpoints) do
      for _, policy in ipairs(endpoint.UserIdentityTokens) do
        -- Select JWT token policy. There also Azure, OAuth2 and OPCUA.
        if  policy.TokenType == ua.UserTokenType.IssuedToken and
            policy.IssuedTokenType == ua.IssuedTokenType.JWT
        then
          tokenPolicy = policy
          goto found
        end
      end
    end

    ::found::
    if not tokenPolicy then
      error("cannot find endpoint with certificate token policy.")
    end

    local jwtHeader = { alg="HS256"}
    local jwtPayload = {
      sub = "1234567890",
      name = "John Doe",
      iat = 1516239022
    }
    local token = require"jwt".sign("my-secret", jwtPayload, jwtHeader)
    local resp, err = client:activateSession(tokenPolicy.PolicyId, token)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_auth_issued_token.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="getting_started.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Getting Started"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="browsing.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Address Space Browsing">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/client/browsing.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="index.html" class="reference internal">Client</a>
  - <a href="getting_started.html" class="reference internal">Getting Started</a>
  - <a href="authentication.html" class="reference internal">Client Authentication</a>
  - <a href="#" class="current reference internal">Address Space Browsing</a>
    - <a href="#browsing-by-string-nodeid" class="reference internal">Browsing by String NodeID</a>
    - <a href="#detailed-parameters" class="reference internal">Detailed Parameters</a>
  - <a href="read_write_data.html" class="reference internal">Reading and Writing Data</a>
  - <a href="adding_nodes.html" class="reference internal">Adding Nodes</a>
  - <a href="http_client.html" class="reference internal">HTTP Client</a>
  - <a href="init.html" class="reference internal">OPC UA Project Code Generator</a>
  - <a href="client_api.html" class="reference internal">Client API</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Client](index.html)
- Address Space Browsing
- <a href="../_sources/client/browsing.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="address-space-browsing" class="section">


# Address Space Browsing<a href="#address-space-browsing" class="headerlink" title="Link to this heading"></a>

Address space browsing is done with the ‘browse’ method. Both the client and server have this method.

The browse method accepts three parameter types:

1.  String with node ID

2.  Array of strings with Node ID

3.  Table with detailed parameters

These parameters are described in section <a href="../types.html#browseparameters" class="reference internal"><span class="std std-ref">BrowseParameters</span></a>.

Method browse returns <a href="../types.html#browseresult" class="reference internal"><span class="std std-ref">BrowseResult</span></a>

<div id="browsing-by-string-nodeid" class="section">

## Browsing by String NodeID<a href="#browsing-by-string-nodeid" class="headerlink" title="Link to this heading"></a>

To browse a single node, you need to pass a string with node ID into the browse method:

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Browse one node by ID.
    resp, err = client:browse(RootFolder)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_browse.lua" class="reference external">Full source</a>

To browse several nodes at once, you can pass an array of nodeIDs as follows:

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Browse array of Node IDs.
    resp, err = client:browse({RootFolder, TypesFolder})

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_browse.lua" class="reference external">Full source</a>

Browsing a node by ID sends a request to the server with the following parameters:

> <div>
>
> - Follow Hierarchical references in forward direction
>
> - Node class mask - All node classes
>
> - Result mask - all attributes
>
> </div>

</div>

<div id="detailed-parameters" class="section">


## Detailed Parameters<a href="#detailed-parameters" class="headerlink" title="Link to this heading"></a>

For special cases, you can manually set the <a href="../types.html#browseparameters" class="reference internal"><span class="std std-ref">browse parameters</span></a> as shown in the following example:

<div class="highlight-lua notranslate">

<div class="highlight">

    local browseParams = {
      NequestedMaxReferencesPerNode = 0,
      NodesToBrowse = {
        {
          NodeId = RootFolder,
          ReferenceTypeId = HierarchicalReferences,
          BrowseDirection = ua.BrowseDirection.Forward,
          NodeClassMask = ua.NodeClass.Unspecified,
          ResultMask = ua.BrowseResultMask.All,
          IncludeSubtypes = true,
        },
        {
          NodeId = TypesFolder,
          ReferenceTypeId = HierarchicalReferences,
          BrowseDirection = ua.BrowseDirection.Forward,
          NodeClassMask = ua.NodeClass.Unspecified,
          ResultMask = ua.BrowseResultMask.All,
          IncludeSubtypes = true,
        }
      },
    }

    local resp,err = client:browse(browseParams)
    for _,res in ipairs(resp.Results) do
      if res.StatusCode ~= ua.StatusCode.Good then
        trace(string.format("Cannot browse node: 0x%X", res.StatusCode))
      else
        trace("References:")
        for i,ref in ipairs(res.References) do
          trace(string.format("%d: NodeId=%s Name=%s", i, ref.NodeId, ref.DisplayName.text))
        end
      end
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_browse.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="authentication.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Client Authentication"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="read_write_data.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Reading and Writing Data">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/client/client_api.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="index.html" class="reference internal">Client</a>
  - <a href="getting_started.html" class="reference internal">Getting Started</a>
  - <a href="authentication.html" class="reference internal">Client Authentication</a>
  - <a href="browsing.html" class="reference internal">Address Space Browsing</a>
  - <a href="read_write_data.html" class="reference internal">Reading and Writing Data</a>
  - <a href="adding_nodes.html" class="reference internal">Adding Nodes</a>
  - <a href="http_client.html" class="reference internal">HTTP Client</a>
  - <a href="init.html" class="reference internal">OPC UA Project Code Generator</a>
  - <a href="#" class="current reference internal">Client API</a>
    - <a href="#constructor" class="reference internal">Constructor</a>
      - <a href="#ua.newClient" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newClient()</code></span></a>
      - <a href="#client-configuration-table" class="reference internal">Client configuration table</a>
    - <a href="#client-connect" class="reference internal">client:connect</a>
    - <a href="#client-opensecurechannel" class="reference internal">client:openSecureChannel</a>
    - <a href="#client-findservers" class="reference internal">client:findServers</a>
    - <a href="#client-getendpoints" class="reference internal">client:getEndpoints</a>
    - <a href="#client-createsession" class="reference internal">client:createSession</a>
    - <a href="#client-activatesession" class="reference internal">client:activateSession</a>
    - <a href="#client-browse" class="reference internal">client:browse</a>
    - <a href="#client-read" class="reference internal">client:read</a>
    - <a href="#client-write" class="reference internal">client:write</a>
    - <a href="#client-addnodes" class="reference internal">client:addNodes</a>
    - <a href="#client-translatebrowsepaths" class="reference internal">client:translateBrowsePaths</a>
    - <a href="#client-createsubscription" class="reference internal">client:createSubscription</a>
    - <a href="#client-call" class="reference internal">client:call</a>
    - <a href="#client-renewsecurechannel" class="reference internal">client:renewSecureChannel</a>
    - <a href="#client-checksecurechannel" class="reference internal">client:checkSecureChannel</a>
    - <a href="#client-connected" class="reference internal">client:connected</a>
    - <a href="#client-closesession" class="reference internal">client:closeSession</a>
    - <a href="#client-closesecurechannel" class="reference internal">client:closeSecureChannel</a>
    - <a href="#client-disconnect" class="reference internal">client:disconnect</a>
    - <a href="#message-callback" class="reference internal">Message callback</a>
      - <a href="#msgCallback" class="reference internal"><span class="pre"><code class="docutils literal notranslate">msgCallback()</code></span></a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Client](index.html)
- Client API
- <a href="../_sources/client/client_api.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="client-api" class="section">

# Client API<a href="#client-api" class="headerlink" title="Link to this heading"></a>

<div id="constructor" class="section">

## Constructor<a href="#constructor" class="headerlink" title="Link to this heading"></a>

A client is created by calling:

<span class="sig-prename descclassname"><span class="pre">ua.</span></span><span class="sig-name descname"><span class="pre">newClient</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">config</span></span>*, *<span class="n"><span class="pre">model</span></span>*<span class="sig-paren">)</span><a href="#ua.newClient" class="headerlink" title="Link to this definition"></a>  
Config<span class="colon">:</span>  
<a href="#client-configuration-table" class="reference internal"><span class="std std-ref">Client configuration table</span></a>. If **nil** then default configuration is used with the following options:

- One secure policy **None**

- Endpoint URL <span class="pre">`opc.tcp://<ip_address>:4841`</span>. IP address is detected automatically. If detection fails, <span class="pre">`localhost`</span> is used.

Model<span class="colon">:</span>  
<a href="../model/index.html#address-space-api" class="reference internal"><span class="std std-ref">Address Space API</span></a> to use. If **nil** then <a href="../model/model.html#ua.baseModel" class="reference internal" title="ua.baseModel"><span class="pre"><code class="sourceCode python">ua.baseModel()</code></span></a> will be initialized.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local config = {
      applicationName = 'RealTimeLogic example',
      applicationUri = "urn:opcua-lua:example",
      productUri = "urn:opcua-lua:example",
      securePolicies = {
        { -- #1
          securityPolicyUri = ua.SecurityPolicy.None
        }
      }
    }

    local client = ua.newClient(config)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_connect.lua" class="reference external">Full source</a>

<div id="client-configuration-table" class="section">

### Client configuration table<a href="#client-configuration-table" class="headerlink" title="Link to this heading"></a>

**config** - a table with the following:  
**applicationName** (string)  
The client’s application name

**applicationUri** (string)  
application URI

**productUri**  
Product URI

**securePolicies** (table)  
List of policies that are used to secure messages See the <a href="../server/server_config.html#config-table" class="reference internal"><span class="std std-ref">Configuration table</span></a> for details.

**cosocketMode** (bool)  
Flag that sets the client in cosocket mode (also known as asynchronous mode).

**bufSize** (uint32, optional, default=8192)  
Size of internal buffer used for sending and receiving messages.

**logging** (table, optional)  
Optional client logging. No logging is performed if not set. See the <a href="../server/server_config.html#config-table" class="reference internal"><span class="std std-ref">Configuration table</span></a> for details.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    local config = {
      applicationName = 'RealTimeLogic example',
      applicationUri = "urn:opcua-lua:example",
      productUri = "urn:opcua-lua:example",
      securePolicies = {
        { -- #1
          securityPolicyUri = ua.SecurityPolicy.None
        }
      }
    }

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_connect.lua" class="reference external">Full source</a>

</div>

</div>

<div id="client-connect" class="section">

## client:connect<a href="#client-connect" class="headerlink" title="Link to this heading"></a>

Establish connection to a server.

<span class="sig-name descname"><span class="pre">client:connect(endpointUrl\[,</span> <span class="pre">transportProfile\]\[,</span> <span class="pre">connectCallback\])</span></span>  
EndpointUrl<span class="colon">:</span>  
(string) OPCUA server endpoint URL. it is possible to use the following formats:

- <span class="pre">`opc.tcp://hostname:port/path`</span>

- <span class="pre">`opc.http://hostname:port/path`</span>

- <span class="pre">`opc.https://hostname:port/path`</span>

- <span class="pre">`http://hostname:port/path`</span>

- <span class="pre">`https://hostname:port/path`</span>

Some servers might require ‘http(s)://’ scheme instead of ‘opc.http(s)://’.

TransportProfile<span class="colon">:</span>  
(string) Transport profile to use. It defines the encoding of sending and receiving messages. There are two kinds of encoding possible to specify: binary and JSON.

If this parameter is omitted, the client will use binary encoding.

Possible values:  
- <span class="pre">`ua.TranportProfileUri.TcpBinary`</span> is used with the <span class="pre">`opc.tcp`</span> scheme.

- <span class="pre">`ua.TranportProfileUri.HttpsBinary`</span>

- <span class="pre">`ua.TranportProfileUri.HttpsJson`</span>

ConnectCallback<span class="colon">:</span>  
function that will be called on success or error.

Returns<span class="colon">:</span>  
error

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    local client = ua.newClient(config)
    local function connectCallback(err)
      done = true
      if err ~= nil then
        trace("connection failed: "..err)
        return
      end
      trace("Connected sucessfully")
      client:disconnect()
    end

    local function connectToServer()
      trace("connecting to server")
      local endpointUrl = "opc.tcp://localhost:4841"
      client:connect(endpointUrl, connectCallback)
    end

    ba.socket.event(connectToServer, "s")

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_connect_callback.lua" class="reference external">Full source</a>

Example without callback:

<div class="highlight-lua notranslate">

<div class="highlight">

    local client = ua.newClient(config)
    trace("connecting to server")
    local endpointUrl = "opc.tcp://localhost:4841"
    local err = client:connect(endpointUrl)
    if err ~= nil then
      trace("connection failed: "..err)
    else
      trace("Connected sucessfully")
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_connect.lua" class="reference external">Full source</a>

Example of connecting to server over HTTP and with JSON encoding:

<div class="highlight-lua notranslate">

<div class="highlight">

    local err = client:connect("opc.https://localhost:"..mako.sslport.."/opcua/", ua.TranportProfileUri.HttpsJson)
    if err ~= nil then
      error("connection failed: "..err)
    end

    trace("Connected sucessfully")

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_connect_https_json.lua" class="reference external">Full source</a>

</div>

<div id="client-opensecurechannel" class="section">

## client:openSecureChannel<a href="#client-opensecurechannel" class="headerlink" title="Link to this heading"></a>

Open new secure channel. The client must open at least one channel.

<span class="sig-name descname"><span class="pre">client:openSecureChannel(timeoutMs,</span> <span class="pre">securityPolicyUri,</span> <span class="pre">securityMode\[,</span> <span class="pre">remoteCert\]\[,</span> <span class="pre">msgCallback\])</span></span>  
TimeoutMs<span class="colon">:</span>  
(uint32) How long a channel should be alive/active (in milliseconds).

SecurityPolicyUri<span class="colon">:</span>  
(string) Security policy URI. Use constants such as <span class="pre">`ua.SecurityPolicy.None`</span>, <span class="pre">`ua.SecurityPolicy.Basic128Rsa15`</span>, or <span class="pre">`ua.SecurityPolicy.Basic256Sha256`</span>.

SecurityMode<span class="colon">:</span>  
(number) Message security mode. Use <span class="pre">`ua.MessageSecurityMode.None`</span>, <span class="pre">`ua.MessageSecurityMode.Sign`</span>, or <span class="pre">`ua.MessageSecurityMode.SignAndEncrypt`</span>.

RemoteCert<span class="colon">:</span>  
Optional remote server certificate. Required by secure policies that need the server certificate before the session response supplies it.

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
<a href="../types.html#opensecurechannelresponse" class="reference internal"><span class="std std-ref">OpenSecureChannelResponse</span></a>, error

The client automatically schedules secure-channel renewal after a channel has been established. For <span class="pre">`opc.http`</span>, <span class="pre">`opc.https`</span>, <span class="pre">`http`</span>, and <span class="pre">`https`</span> connections, the transport has no OPC UA TCP secure channel; the method stores the selected policy/mode and calls the callback, if one is provided.

Callback example:

<div class="highlight-lua notranslate">

<div class="highlight">

    local client = ua.newClient(config)

    local function onChannelOpened(resp, err)
      if err ~= nil then
        trace("Secure channel error: "..tostring(err))
        return
      end
      trace("Opened secure channel with id: "..resp.SecurityToken.ChannelId)
      done = true
    end

    local function connectCallback(err)
      if err == nil then
        local secureChannelTimeout = 60000 -- ms
        client:openSecureChannel(secureChannelTimeout, ua.SecurityPolicy.None, ua.MessageSecurityMode.None, nil, onChannelOpened)
      end
    end

    local function connectToServer()
      trace("connecting to server")
      local endpointUrl = "opc.tcp://localhost:4841"
      client:connect(endpointUrl, connectCallback)
    end

    ba.socket.event(connectToServer, "s")

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_open_secure_channel_callback.lua" class="reference external">Full source</a>

Blocking example (no callback):

<div class="highlight-lua notranslate">

<div class="highlight">

    local resp, err = client:openSecureChannel(120000, ua.SecurityPolicy.None, ua.MessageSecurityMode.None)
    if err ~= nil then
      trace("Opening secure channel failed: "..err)
    else
      trace("Opened secure channel with id: "..resp.SecurityToken.ChannelId)
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_open_secure_channel.lua" class="reference external">Full source</a>

</div>

<div id="client-findservers" class="section">

## client:findServers<a href="#client-findservers" class="headerlink" title="Link to this heading"></a>

Get the list of the servers known to a Server or Discovery Server

<span class="sig-name descname"><span class="pre">client:findServers(params\[,</span> <span class="pre">msgCallback\])</span></span>  
Params<span class="colon">:</span>  
table with fields: **EndpointUrl** (string, optional)

> <div>
>
> Endpoint URL.
>
> </div>

**LocaleIds\[\]** (string, optional)

> <div>
>
> List of locales to use. The Server should return the applicationName in the ApplicationDescription using one of locales specified.
>
> </div>

**ServerUris\[\]** (string, optional)  
The List of servers to return. All known servers are returned if the list is empty.

**msgCallback** - <a href="#message-callback" class="reference internal">Message callback</a>

**Result**: <a href="../types.html#findserversresponse" class="reference internal"><span class="std std-ref">FindServersResponse</span></a>, error

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Select known servers
    local params = {
      EndpointUrl = "opc.tcp://localhost:4841"
    }

    local resp, err = client:findServers(params)
    if err ~= nil then
      trace("Find servers error: "..err)
    else
      if not resp.Servers[0] then
        trace("No servers found.")
      end
      for i,srv in ipairs(resp.Servers) do
        trace("server #"..i)
        trace("  "..srv.ApplicationUri)
        trace("  "..srv.ProductUri)
        trace("  "..srv.ApplicationName.Text)
      end
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_find_servers.lua" class="reference external">Full source</a>

</div>

<div id="client-getendpoints" class="section">

## client:getEndpoints<a href="#client-getendpoints" class="headerlink" title="Link to this heading"></a>

Returns the Endpoints supported by a Server and all of the configuration information required to establish a SecureChannel and a Session.

<span class="sig-name descname"><span class="pre">client:getEndpoints(params\[,</span> <span class="pre">msgCallback\])</span></span>  
Params<span class="colon">:</span>  
a table with the following fields:

**EndpointUrl** (string, optional)  
The network address the Client used when accessing the DiscoveryEndpoint.

**LocaleIds\[\]** (string, optional)  
List of locales to use. Specifies the locale to use when returning human readable strings.

**ProfileUris\[\]** (string, optional)  
List of Transport Profiles that the returned Endpoints support.

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
<a href="../types.html#getendpointsresponse" class="reference internal"><span class="std std-ref">GetEndpointsResponse</span></a>,error

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Select endpoints
    local params = {
      EndpointUrl = "opc.tcp://localhost:4841"
    }

    local resp, err = client:getEndpoints(params)
    if err ~= nil then
      trace("Get endpoints error: "..err)
    else
      if not resp.Endpoints[0] then
        trace("No endpoints found.")
      end
      for i,endpoint in ipairs(resp.Endpoints) do
        trace("enspoint #"..i)
        trace("  "..endpoint.EndpointUrl)
        trace("  "..endpoint.TransportProfileUri)
        trace("  "..endpoint.SecurityPolicyUri)
      end
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_get_endpoints.lua" class="reference external">Full source</a>

</div>

<div id="client-createsession" class="section">

## client:createSession<a href="#client-createsession" class="headerlink" title="Link to this heading"></a>

This Service is used by an OPC UA Client when creating a Session. The Server returns two values which uniquely identifies the Session.

<span class="sig-name descname"><span class="pre">client:createSession(name,</span> <span class="pre">timeoutMs\[,</span> <span class="pre">msgCallback\])</span></span>  
<span class="sig-name descname"><span class="pre">client:createSession(params\[,</span> <span class="pre">msgCallback\])</span></span>  
Name<span class="colon">:</span>  
(string) Human readable string identifying the Session.

TimeoutMs<span class="colon">:</span>  
(double) Requested maximum number of milliseconds that a Session should remain open without activity.

Params<span class="colon">:</span>  
a table with the following fields:

**ApplicationUri** (string)  
Client application URI.

**ProductUri** (string)  
Client product URI.

**ApplicationName** (string)  
Human readable client application name.

**ApplicationType** (number)  
Application type, for example <span class="pre">`ua.ApplicationType.Client`</span>.

**ServerUri** (string, optional)  
Server URI.

**EndpointUrl** (string)  
Endpoint URL used for the session.

**SessionName** (string)  
Human readable session name.

**SessionTimeout** (double)  
Requested session timeout in milliseconds.

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
<a href="../types.html#createsessionresponse" class="reference internal"><span class="std std-ref">CreateSessionResponse</span></a>, error

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    resp, err = client:createSession("test_session", 3600000)
    if err ~= nil then
      trace("Creating session failed: "..err)
      return
    end

    trace("created session:")
    trace("  sessionId='"..resp.SessionId.."'")
    trace("  authenticationToken='"..resp.AuthenticationToken.."'")
    trace("  revisedSessionTimeout='"..resp.RevisedSessionTimeout.."'")

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_create_session.lua" class="reference external">Full source</a>

</div>

<div id="client-activatesession" class="section">

## client:activateSession<a href="#client-activatesession" class="headerlink" title="Link to this heading"></a>

Activate a previously created session. This method authenticates the user with one of the token policies returned by the server endpoint description.

<span class="sig-name descname"><span class="pre">client:activateSession(\[msgCallback\])</span></span>  
<span class="sig-name descname"><span class="pre">client:activateSession(policyId,</span> <span class="pre">token\[,</span> <span class="pre">token2\]\[,</span> <span class="pre">msgCallback\])</span></span>  
<span class="sig-name descname"><span class="pre">client:activateSession(params\[,</span> <span class="pre">msgCallback\])</span></span>  
PolicyId<span class="colon">:</span>  
(string)

Token policy to use. This is taken from the server’s endpoint description. Endpoint description can be obtained by calling <span class="pre">`client:getEndpoints()`</span> or <span class="pre">`client:createSession()`</span>.

Token<span class="colon">:</span>  
(string) User identity token data. For username authentication this is the user name. For issued-token authentication this is the token. For certificate authentication this is the user certificate.

Token2<span class="colon">:</span>  
(string) Second token value when required. For username authentication this is the password. For certificate authentication this is the private key.

Params<span class="colon">:</span>  
(table) Manual <span class="pre">`ActivateSession`</span> request body. Use this form when constructing <span class="pre">`ClientSignature`</span>, <span class="pre">`UserIdentityToken`</span>, <span class="pre">`UserTokenSignature`</span>, or <span class="pre">`Locales`</span> yourself.

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
<a href="../types.html#activatesessionresponse" class="reference internal"><span class="std std-ref">ActivateSessionResponse</span></a>, error

Calling <span class="pre">`client:activateSession()`</span> with no policy arguments selects the anonymous token policy. Passing a <span class="pre">`policyId`</span> selects the matching token policy from the endpoint returned during session creation.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    resp, err = client:activateSession()
    if err ~= nil then
      trace("Activating session failed: "..err)
      return
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_create_session.lua" class="reference external">Full source</a>

</div>

<div id="client-browse" class="section">

## client:browse<a href="#client-browse" class="headerlink" title="Link to this heading"></a>

This Service is used for discovering the References of a specified Node.

<span class="sig-name descname"><span class="pre">client:browse(\[nodeId</span> <span class="pre">\|</span> <span class="pre">nodeId\[\]</span> <span class="pre">\|</span> <span class="pre">params\]</span> <span class="pre">\[,</span> <span class="pre">msgCallback\])</span></span>  
NodeId<span class="colon">:</span>  
(<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>) id of the node to browse.

NodeId\[\]<span class="colon">:</span>  
(<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>) array of NodeIDs to browse

Params<span class="colon">:</span>  
(table) <a href="../types.html#browseparameters" class="reference internal"><span class="std std-ref">BrowseParameters</span></a>

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
<a href="../types.html#browseresult" class="reference internal"><span class="std std-ref">Browsing Result</span></a>, error

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Browse one node by ID.
    resp, err = client:browse(RootFolder)
    if err ~= nil then
      return
    end

    for _,res in ipairs(resp.Results) do
      if res.StatusCode ~= ua.StatusCode.Good then
        trace(string.format("Cannot browse node: 0x%X", res.StatusCode))
      else
        trace("References:")
        for i,ref in ipairs(res.References) do
          trace(string.format("%d: NodeId=%s Name=%s", i, ref.NodeId, ref.DisplayName.Text))
        end
      end
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_browse.lua" class="reference external">Full source</a>

</div>

<div id="client-read" class="section">

## client:read<a href="#client-read" class="headerlink" title="Link to this heading"></a>

Read one or more Attributes of one or more Nodes.

<span class="sig-name descname"><span class="pre">client:read(\<nodeId</span> <span class="pre">\|</span> <span class="pre">nodeId\[\]</span> <span class="pre">\|</span> <span class="pre">params\></span> <span class="pre">\[,</span> <span class="pre">msgCallback\])</span></span>  
NodeId<span class="colon">:</span>  
(<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>) Read possible attributes of one node by NodeId.

NodeId\[\]<span class="colon">:</span>  
(<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>) Array of NodeIds to read. All possible attributes will be read

Params<span class="colon">:</span>  
Table with detailed parameters. For details see <a href="read_write_data.html#read-attributes" class="reference internal"><span class="std std-ref">Reading Attributes</span></a>

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
The result from an OPC UA call will be an array. Every element of the array will be a table with two fields: Status code for the current node and the value of the attribute.

Every element of the array contains a table with the following fields:

**StatusCode**  
The status code from reading the corresponding node.

**Value**  
The value of the attribute. The value will be nil in case of error.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    resp,err = client:read(ObjectsFolder)
    for i,result in ipairs(resp.Results) do
      if result.StatusCode == 0 then
        ua.printTable("result", result.Value)
      else
        trace(string.format("Read attributes error: 0x%X", result.StatusCode))
      end
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_read.lua" class="reference external">Full source</a>

</div>

<div id="client-write" class="section">

## client:write<a href="#client-write" class="headerlink" title="Link to this heading"></a>

This Service is used when writing values to one or more Attributes of one or more Nodes.

<span class="sig-name descname"><span class="pre">client:write(params\[,</span> <span class="pre">msgCallback\])</span></span>  
Params<span class="colon">:</span>  
(table)

**NodesToWrite\[\]** (array)

> <div>
>
> **NodeId** (<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>) node identifier
>
> **AttributeId** (<a href="read_write_data.html#attribute" class="reference internal"><span class="std std-ref">Node Attribute</span></a>) attribute to write
>
> **Value** (<a href="../types.html#data-value-type" class="reference internal"><span class="std std-ref">DataValue</span></a>) New value of attribute
>
> </div>

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
<a href="../types.html#writeresponse" class="reference internal"><span class="std std-ref">WriteResponse</span></a>,error

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Update the OPC-UA server's start time.
    local nodes = {
      NodesToWrite = {
        {
          NodeId = Server_ServerStatus_StartTime,
          AttributeId = ua. AttributeId.Value,
          Value = {   -- DataValue
            Type = ua.VariantType.DateTime,
            Value = 0.0,
            StatusCode = ua.StatusCode.Good
          }
        }
      }
    }

    local resp,err = client:write(nodes)
    if resp.Results[1] ~= 0 then
      trace(string.format("Changing attribute value failed: 0x%X", resp.Results[1]))
    else
      trace(string.format("Attribute value changed sucessfully"))
    end

    client:disconnect()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_write.lua" class="reference external">Full source</a>

</div>

<div id="client-addnodes" class="section">

## client:addNodes<a href="#client-addnodes" class="headerlink" title="Link to this heading"></a>

Add one or more Nodes into the AddressSpace hierarchy.

<span class="sig-name descname"><span class="pre">client:addNodes(parameters\[,</span> <span class="pre">msgCallback\])</span></span>  
Parameters<span class="colon">:</span>  
(table) A table with array of nodes to add. See details in <a href="adding_nodes.html#adding-nodes" class="reference internal"><span class="std std-ref">Adding Nodes</span></a>

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
<a href="../types.html#addnodesresponse" class="reference internal"><span class="std std-ref">AddNodesResponse</span></a>,error

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    local variableId = "i=1000000"

    local dataValue = {
      Type = ua.VariantType.UInt32,
      Value = 30000,
      StatusCode = ua.StatusCode.Good
    }

    local newVariable = ua.newVariableParams(ObjectsFolder, "UInt32", dataValue, variableId)

    local request = {
      NodesToAdd = {newVariable}
    }

    resp, err = client:addNodes(request)
    for i,res in ipairs(resp.results) do
      if res.statusCode ~= 0 then
        trace(string.format("Adding variable node failed: 0x%X", res.statusCode))
      else
        trace(string.format("Added new variable with NodeId: '%s'", res.addedNodeId))
      end
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_add_nodes.lua" class="reference external">Full source</a>

</div>

<div id="client-translatebrowsepaths" class="section">

## client:translateBrowsePaths<a href="#client-translatebrowsepaths" class="headerlink" title="Link to this heading"></a>

This Service is used when requesting that the Server translates one or more browse paths to NodeIds.

<span class="sig-name descname"><span class="pre">client:translateBrowsePaths(params\[,</span> <span class="pre">msgCallback\])</span></span>  
Params<span class="colon">:</span>  
(table) TranslateBrowsePathsToNodeIds request parameters.

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
TranslateBrowsePathsToNodeIds response, error

</div>

<div id="client-createsubscription" class="section">

## client:createSubscription<a href="#client-createsubscription" class="headerlink" title="Link to this heading"></a>

Create a subscription on the active session.

<span class="sig-name descname"><span class="pre">client:createSubscription(params\[,</span> <span class="pre">msgCallback\])</span></span>  
Params<span class="colon">:</span>  
(table) CreateSubscription request parameters.

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
CreateSubscription response, error

</div>

<div id="client-call" class="section">

## client:call<a href="#client-call" class="headerlink" title="Link to this heading"></a>

Call a method node on the server.

<span class="sig-name descname"><span class="pre">client:call(objectId,</span> <span class="pre">methodId,</span> <span class="pre">inputArguments\[,</span> <span class="pre">msgCallback\])</span></span>  
ObjectId<span class="colon">:</span>  
(<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>) NodeId of the object that owns the method.

MethodId<span class="colon">:</span>  
(<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>) NodeId of the method to call.

InputArguments<span class="colon">:</span>  
(array) Input argument values for the method.

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
Call response, error

</div>

<div id="client-renewsecurechannel" class="section">

## client:renewSecureChannel<a href="#client-renewsecurechannel" class="headerlink" title="Link to this heading"></a>

Renew the current OPC UA TCP secure channel token.

<span class="sig-name descname"><span class="pre">client:renewSecureChannel(timeoutMs\[,</span> <span class="pre">msgCallback\])</span></span>  
TimeoutMs<span class="colon">:</span>  
(uint32) Requested token lifetime in milliseconds.

MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
<a href="../types.html#opensecurechannelresponse" class="reference internal"><span class="std std-ref">OpenSecureChannelResponse</span></a>, error

</div>

<div id="client-checksecurechannel" class="section">

## client:checkSecureChannel<a href="#client-checksecurechannel" class="headerlink" title="Link to this heading"></a>

Renew the secure channel if the internal renewal timer has marked it as stale.

<span class="sig-name descname"><span class="pre">client:checkSecureChannel()</span></span>  
Returns<span class="colon">:</span>  
error, or <span class="pre">`nil`</span> when no renewal was required or renewal succeeded.

</div>

<div id="client-connected" class="section">

## client:connected<a href="#client-connected" class="headerlink" title="Link to this heading"></a>

Check whether the underlying transport is connected.

<span class="sig-name descname"><span class="pre">client:connected()</span></span>  
Returns<span class="colon">:</span>  
<span class="pre">`true`</span> when connected, otherwise <span class="pre">`false`</span>.

</div>

<div id="client-closesession" class="section">

## client:closeSession<a href="#client-closesession" class="headerlink" title="Link to this heading"></a>

Terminate an active Session.

<span class="sig-name descname"><span class="pre">client:closeSession(\[msgCallback\])</span></span>  
MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
<a href="../types.html#closesessionresponse" class="reference internal"><span class="std std-ref">CloseSessionResponse</span></a>,error

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    resp, err = client:closeSession()
    if err == nil then
      trace("Session closed")
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_create_session.lua" class="reference external">Full source</a>

</div>

<div id="client-closesecurechannel" class="section">

## client:closeSecureChannel<a href="#client-closesecurechannel" class="headerlink" title="Link to this heading"></a>

Terminate a SecureChannel.

<span class="sig-name descname"><span class="pre">client:closeSecureChannel(\[msgCallback\])</span></span>  
MsgCallback<span class="colon">:</span>  
<a href="#message-callback" class="reference internal">Message callback</a>

Returns<span class="colon">:</span>  
<a href="../types.html#closesecurechannelresponse" class="reference internal"><span class="std std-ref">CloseSecureChannelResponse</span></a>,error

</div>

<div id="client-disconnect" class="section">

## client:disconnect<a href="#client-disconnect" class="headerlink" title="Link to this heading"></a>

Close the client’s server socket connection. Calling this method also closes any open channel.

<span class="sig-name descname"><span class="pre">client:disconnect()</span></span>  
Returns<span class="colon">:</span>  
response, error

</div>

<div id="message-callback" class="section">

## Message callback<a href="#message-callback" class="headerlink" title="Link to this heading"></a>

Callback function that is called when the request has been completed.

<span class="sig-name descname"><span class="pre">msgCallback</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">response</span></span>*, *<span class="n"><span class="pre">err</span></span>*<span class="sig-paren">)</span><a href="#msgCallback" class="headerlink" title="Link to this definition"></a>  
Response<span class="colon">:</span>  
Data received from server in response to corresponding request.

Err<span class="colon">:</span>  
Any error that occurred during processing of request.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="init.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="OPC UA Project Code Generator"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="../server/index.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Server">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/client/getting_started.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="index.html" class="reference internal">Client</a>
  - <a href="#" class="current reference internal">Getting Started</a>
    - <a href="#connecting-to-server" class="reference internal">Connecting to Server</a>
    - <a href="#browsing-address-space" class="reference internal">Browsing Address Space</a>
    - <a href="#reading-node-attributes" class="reference internal">Reading Node Attributes</a>
    - <a href="#closing-server-connection" class="reference internal">Closing Server Connection</a>
    - <a href="#asynchronous-cosocket-mode" class="reference internal">Asynchronous Cosocket Mode</a>
  - <a href="authentication.html" class="reference internal">Client Authentication</a>
  - <a href="browsing.html" class="reference internal">Address Space Browsing</a>
  - <a href="read_write_data.html" class="reference internal">Reading and Writing Data</a>
  - <a href="adding_nodes.html" class="reference internal">Adding Nodes</a>
  - <a href="http_client.html" class="reference internal">HTTP Client</a>
  - <a href="init.html" class="reference internal">OPC UA Project Code Generator</a>
  - <a href="client_api.html" class="reference internal">Client API</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Client](index.html)
- Getting Started
- <a href="../_sources/client/getting_started.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="getting-started" class="section">

# Getting Started<a href="#getting-started" class="headerlink" title="Link to this heading"></a>

An OPC UA client is a network client that interacts with an OPC UA server via a collection of abstract Remote Procedure Calls (RPC), which is a request/response protocol. An RPC is initiated by the client, which sends a request message to the server. The OPC UA server then sends a response to the client, and the application continues its process. While the OPC UA server is processing the call, the OPC UA client is blocked (it waits until the server has finished processing before resuming execution), unless the OPC UA client operates in asynchronous callback mode.

The OPC UA client can operate in standard blocking socket call (default) or nonblocking cosocket mode. Blocking sockets must run in the context of a native thread, such as a thread running an LSP page. See <a href="#cosocketmode" class="reference internal"><span class="std std-ref">Asynchronous Cosocket Mode</span></a> for details.

<div id="connecting-to-server" class="section">

## Connecting to Server<a href="#connecting-to-server" class="headerlink" title="Link to this heading"></a>

A client is created by first loading the OPC-UA stack and then creating a client as follows:

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local config = {
      applicationName = 'RealTimeLogic example',
      applicationUri = "urn:opcua-lua:example",
      productUri = "urn:opcua-lua:example",
      securePolicies = {
        { -- #1
          securityPolicyUri = ua.SecurityPolicy.None
        }
      },
    }

    local client = ua.newClient(config)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_create_session.lua" class="reference external">Full source</a>

Next, connect to the server, open an OPC-UA channel and finally create an OPC-UA session:

<div class="highlight-lua notranslate">

<div class="highlight">

    local resp, err
    local endpointUrl = "opc.tcp://localhost:4841"
    client:connect(endpointUrl, connectCallback)
    resp, err = client:openSecureChannel(120000, ua.SecurityPolicy.None, ua.MessageSecurityMode.None)
    resp, err = client:createSession("test_session", 3600000)
    resp, err = client:activateSession()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_create_session.lua" class="reference external">Full source</a>

You can start browsing the address space as soon as a session has been established.

</div>

<div id="browsing-address-space" class="section">

## Browsing Address Space<a href="#browsing-address-space" class="headerlink" title="Link to this heading"></a>

Call the ‘browse’ method to browse the server’s address space.

<div class="highlight-lua notranslate">

<div class="highlight">

    local RootFolder = "i=84"
    local BaseDataVariableType = "i=63"
    local FolderType = "i=61"
    local ObjectsFolder = "i=85"
    local Organizes = "i=35"
    local UInt32 = "i=24"
    local HierarchicalReferences = "i=33"
    local TypesFolder = "i=86"


    -- Browse one node by ID.
    resp, err = client:browse(RootFolder)

    -- Browse array of Node IDs.
    resp, err = client:browse({RootFolder, TypesFolder})
    if err ~= nil then
      return
    end

    for _,res in ipairs(resp.Results) do
      if res.StatusCode ~= ua.StatusCode.Good then
        trace(string.format("Cannot browse node: 0x%X", res.StatusCode))
      else
        trace("References:")
        for i,ref in ipairs(res.References) do
          trace(string.format("%d: NodeId=%s Name=%s", i, ref.NodeId, ref.DisplayName.Text))
        end
      end
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_browse.lua" class="reference external">Full source</a>

The <a href="browsing.html#browsing" class="reference internal"><span class="std std-ref">Address Space Browsing</span></a> user guide has a detailed overview of all parameters.

</div>

<div id="reading-node-attributes" class="section">

## Reading Node Attributes<a href="#reading-node-attributes" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local config = {
    local ObjectsFolder = "i=85"
    local TypesFolder = "i=86"

    -- Read all possible attributes of the any node
    -- For part of attributes will be returned a valus
    -- and for part of attributes will be returned a status code BadAttributeIdInvalid
    resp,err = client:read(ObjectsFolder)
    for i,result in ipairs(resp.Results) do
      if result.StatusCode == 0 then
        ua.printTable("result", result.Value)
      else
        trace(string.format("Read attributes error: 0x%X", result.StatusCode))
      end
    end

    -- Read all possible attributes of several nodes
    -- For part of attributes will be returned a valus
    -- and for part of attributes will be returned a status code BadAttributeIdInvalid
    resp,err = client:read({ObjectsFolder, TypesFolder})

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_read.lua" class="reference external">Full source</a>

See <a href="read_write_data.html#read-write-data" class="reference internal"><span class="std std-ref">Reading and Writing Data</span></a> for parameters and detailed information.

</div>

<div id="closing-server-connection" class="section">

## Closing Server Connection<a href="#closing-server-connection" class="headerlink" title="Link to this heading"></a>

When finished using the server, close the server connection as follows:

<div class="highlight-lua notranslate">

<div class="highlight">

    resp, err = client:closeSession()
    client:disconnect()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_create_session.lua" class="reference external">Full source</a>

</div>

<div id="asynchronous-cosocket-mode" class="section">


## Asynchronous Cosocket Mode<a href="#asynchronous-cosocket-mode" class="headerlink" title="Link to this heading"></a>

Clients, by default, use blocking sockets. However, a client can be configured to operate in cosocket mode, a lightweight Lua coroutine socket thread. All client methods can accept a callback that will be called when the corresponding request completes. The OPC UA methods can also be called without providing a callback when running in cosocket mode if the caller is running in the context of another cosocket. When another cosocket calls an OPC UA client in cosocket mode without providing a callback, the calling cosocket will block. Not having to provide a callback when operating in cosocket mode makes designing OPC UA clients easier.

We recommend reading the Barracuda App Server’s <a href="../doc/?url=SockLib.html" class="reference external">Socket Design User Guide</a> for an introduction to cosockets and for details on the various socket modes.

The cosocket mode is set in the configuration table with the flag ‘cosocketMode’.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local config = {
      applicationName = 'RealTimeLogic example',
      applicationUri = "urn:opcua-lua:example",
      productUri = "urn:opcua-lua:example",
      securePolicies = {
        { -- #1
          securityPolicyUri = ua.SecurityPolicy.None
        }
      },
      cosocketMode = true, -- Start socket read loop in separate cosocket
    }
    local client = ua.newClient(config)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_cosock_callback.lua" class="reference external">Full source</a>

All callbacks, except for the connect method, must accept two arguments. The connect method receives one argument as shown in the two examples below.

<div class="highlight-lua notranslate">

<div class="highlight">

      local endpointUrl = "opc.tcp://localhost:4841"
      client:connect(endpointUrl, connectCallback)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_cosock_callback.lua" class="reference external">Full source</a>

The connect method’s error argument is nil if the connection succeeded.

<div class="highlight-lua notranslate">

<div class="highlight">

    local function connectCallback(err)
      if err ~= nil then
        trace("connection error: "..err)
        return
      end

      trace("Opening secure channel")
      local secureChannelTimeout = 60000 -- ms
      client:openSecureChannel(secureChannelTimeout, ua.SecurityPolicy.None, ua.MessageSecurityMode.None, nil, onChannelOpened)
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_cosock_callback.lua" class="reference external">Full source</a>

All other callbacks receive two arguments: the response and an error code. This corresponds to the two return values returned when operating in blocking mode (when not providing a callback). The methods return <span class="pre">`response,`</span>` `<span class="pre">`nil`</span> on success and <span class="pre">`nil,`</span>` `<span class="pre">`error`</span> on failure.

<div class="highlight-lua notranslate">

<div class="highlight">

    local function onChannelOpened(resp, err)
      if err ~= nil then
        trace("Secure channel error: "..tostring(err))
        return
      end
      ua.printTable("SecureChannel", resp)

      trace("Disconnecting from server")
      pcall(client.disconnect, client)
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_cosock_callback.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="index.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Client"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="authentication.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Client Authentication">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/client/http_client.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="index.html" class="reference internal">Client</a>
  - <a href="getting_started.html" class="reference internal">Getting Started</a>
  - <a href="authentication.html" class="reference internal">Client Authentication</a>
  - <a href="browsing.html" class="reference internal">Address Space Browsing</a>
  - <a href="read_write_data.html" class="reference internal">Reading and Writing Data</a>
  - <a href="adding_nodes.html" class="reference internal">Adding Nodes</a>
  - <a href="#" class="current reference internal">HTTP Client</a>
    - <a href="#http-examples" class="reference internal">HTTP Examples</a>
      - <a href="#binary-encoding" class="reference internal">Binary Encoding</a>
      - <a href="#json-encoding" class="reference internal">JSON Encoding</a>
  - <a href="init.html" class="reference internal">OPC UA Project Code Generator</a>
  - <a href="client_api.html" class="reference internal">Client API</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Client](index.html)
- HTTP Client
- <a href="../_sources/client/http_client.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="http-client" class="section">

# HTTP Client<a href="#http-client" class="headerlink" title="Link to this heading"></a>

To connect to a server over HTTP, you need to create a client object and call the <span class="pre">`connect`</span> method.

The connect method accepts these parameters:

| Parameter                                   | Description                                                                                                                                                                                                                                                                          |
|---------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <span class="pre">`endpointUrl`</span>      | Server URL. OPC UA HTTP endpoints use <span class="pre">`opc.http://host:port/path`</span> or <span class="pre">`opc.https://host:port/path`</span>. Some servers also accept <span class="pre">`http://host:port/path`</span> or <span class="pre">`https://host:port/path`</span>. |
| <span class="pre">`transportProfile`</span> | Optional transport profile. If omitted, the client tries binary encoding.                                                                                                                                                                                                            |

| Transport profile                                            | URI                                                                                     |
|--------------------------------------------------------------|-----------------------------------------------------------------------------------------|
| <span class="pre">`ua.TranportProfileUri.HttpsJson`</span>   | <span class="pre">`http://opcfoundation.org/UA-Profile/Transport/https-json`</span>     |
| <span class="pre">`ua.TranportProfileUri.HttpsBinary`</span> | <span class="pre">`http://opcfoundation.org/UA-Profile/Transport/https-uabinary`</span> |

<div id="http-examples" class="section">

## HTTP Examples<a href="#http-examples" class="headerlink" title="Link to this heading"></a>

<div id="binary-encoding" class="section">

### Binary Encoding<a href="#binary-encoding" class="headerlink" title="Link to this heading"></a>

The following example shows how to connect to a server using binary encoding over HTTP.

<div class="highlight-lua notranslate">

<div class="highlight">

    local err = client:connect("opc.http://localhost:"..mako.port.."/opcua/", ua.TranportProfileUri.HttpsBinary)
    if err ~= nil then
      error("connection failed: "..err)
    end

    trace("Connected sucessfully")

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_connect_http_binary.lua" class="reference external">Full source</a>

You can also omit the second parameter; the client will try to connect using binary encoding.

<div class="highlight-lua notranslate">

<div class="highlight">

    local err = client:connect("opc.https://localhost:"..mako.sslport.."/opcua/")
    if err ~= nil then
      error("connection failed: "..err)
    end

    trace("Connected sucessfully")

</div>

</div>

To connect to a server over HTTPS, you need to pass URL with opc.https:// scheme.

<div class="highlight-lua notranslate">

<div class="highlight">

    local err = client:connect("opc.https://localhost:"..mako.sslport.."/opcua/", ua.TranportProfileUri.HttpsBinary)
    if err ~= nil then
      error("connection failed: "..err)
    end

    trace("Connected sucessfully")

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_connect_https_binary.lua" class="reference external">Full source</a>

</div>

<div id="json-encoding" class="section">

### JSON Encoding<a href="#json-encoding" class="headerlink" title="Link to this heading"></a>

JSON encoding is a non-standard way to communicate between OPC UA clients and servers. Because of this, you can only connect to RealTimeLogic server using JSON encoding.

The following example shows how to connect to a server using JSON encoding over HTTP.

<div class="highlight-lua notranslate">

<div class="highlight">

    local err = client:connect("opc.https://localhost:"..mako.sslport.."/opcua/", ua.TranportProfileUri.HttpsJson)
    if err ~= nil then
      error("connection failed: "..err)
    end

    trace("Connected sucessfully")

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_connect_https_json.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="adding_nodes.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Adding Nodes"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="init.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="OPC UA Project Code Generator">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/client/index.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="#" class="current reference internal">Client</a>
  - <a href="getting_started.html" class="reference internal">Getting Started</a>
  - <a href="authentication.html" class="reference internal">Client Authentication</a>
  - <a href="browsing.html" class="reference internal">Address Space Browsing</a>
  - <a href="read_write_data.html" class="reference internal">Reading and Writing Data</a>
  - <a href="adding_nodes.html" class="reference internal">Adding Nodes</a>
  - <a href="http_client.html" class="reference internal">HTTP Client</a>
  - <a href="init.html" class="reference internal">OPC UA Project Code Generator</a>
  - <a href="client_api.html" class="reference internal">Client API</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Client
- <a href="../_sources/client/index.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="client" class="section">

# Client<a href="#client" class="headerlink" title="Link to this heading"></a>

Client is a network client that interacts with an OPC UA server via a collection of abstract Remote Procedure Calls (RPC), which is a request/response protocol. An RPC is initiated by the client, which sends a request message to the server. The OPC UA server then sends a response to the client, and the application continues its process. While the OPC UA server is processing the call, the OPC UA client is blocked (it waits until the server has finished processing before resuming execution), unless the OPC UA client operates in asynchronous callback mode.

The OPC UA client can operate in standard blocking socket call (default) or nonblocking cosocket mode. Blocking sockets must run in the context of a native thread, such as a thread running an LSP page. See <a href="getting_started.html#cosocketmode" class="reference internal"><span class="std std-ref">Asynchronous Cosocket Mode</span></a> for details.

<div class="toctree-wrapper compound">

- <a href="getting_started.html" class="reference internal">Getting Started</a>
  - <a href="getting_started.html#connecting-to-server" class="reference internal">Connecting to Server</a>
  - <a href="getting_started.html#browsing-address-space" class="reference internal">Browsing Address Space</a>
  - <a href="getting_started.html#reading-node-attributes" class="reference internal">Reading Node Attributes</a>
  - <a href="getting_started.html#closing-server-connection" class="reference internal">Closing Server Connection</a>
  - <a href="getting_started.html#asynchronous-cosocket-mode" class="reference internal">Asynchronous Cosocket Mode</a>
- <a href="authentication.html" class="reference internal">Client Authentication</a>
  - <a href="authentication.html#working-with-client-sessions" class="reference internal">Working with client sessions</a>
  - <a href="authentication.html#anonymous" class="reference internal">Anonymous</a>
  - <a href="authentication.html#user-name" class="reference internal">User Name</a>
  - <a href="authentication.html#x509-certificate" class="reference internal">x509 certificate</a>
  - <a href="authentication.html#jwt-oauth2-azure" class="reference internal">JWT, OAuth2, Azure</a>
- <a href="browsing.html" class="reference internal">Address Space Browsing</a>
  - <a href="browsing.html#browsing-by-string-nodeid" class="reference internal">Browsing by String NodeID</a>
  - <a href="browsing.html#detailed-parameters" class="reference internal">Detailed Parameters</a>
- <a href="read_write_data.html" class="reference internal">Reading and Writing Data</a>
  - <a href="read_write_data.html#reading-attributes" class="reference internal">Reading Attributes</a>
    - <a href="read_write_data.html#read-parameters" class="reference internal">Read parameters</a>
    - <a href="read_write_data.html#read-attributes-of-one-node" class="reference internal">Read Attributes Of One Node</a>
    - <a href="read_write_data.html#read-attributes-of-several-nodes" class="reference internal">Read Attributes Of Several Nodes</a>
    - <a href="read_write_data.html#read-variables-values" class="reference internal">Read Variables Values</a>
  - <a href="read_write_data.html#writing-attributes" class="reference internal">Writing Attributes</a>
    - <a href="read_write_data.html#write-parameters" class="reference internal">Write Parameters</a>
    - <a href="read_write_data.html#write-example" class="reference internal">Write example</a>
  - <a href="read_write_data.html#node-attribute" class="reference internal">Node Attribute</a>
    - <a href="read_write_data.html#base-node-attribute-numbers" class="reference internal">Base node attribute numbers</a>
    - <a href="read_write_data.html#object-node-attribute-numbers" class="reference internal">Object node attribute numbers</a>
    - <a href="read_write_data.html#object-type-node-attribute-numbers" class="reference internal">Object Type node attribute numbers</a>
    - <a href="read_write_data.html#variable-node-attribute-numbers" class="reference internal">Variable node attribute numbers</a>
    - <a href="read_write_data.html#variable-type-node-attribute-numbers" class="reference internal">Variable Type node attribute numbers</a>
    - <a href="read_write_data.html#reference-type-node-attribute-numbers" class="reference internal">Reference Type node attribute numbers</a>
    - <a href="read_write_data.html#data-type-node-attribute-numbers" class="reference internal">Data Type node attribute numbers</a>
    - <a href="read_write_data.html#method-node-attribute-numbers" class="reference internal">Method node attribute numbers</a>
    - <a href="read_write_data.html#view-node-attribute-numbers" class="reference internal">View node attribute numbers</a>
    - <a href="read_write_data.html#list-of-all-attribute-numbers" class="reference internal">List of all attribute numbers</a>
- <a href="adding_nodes.html" class="reference internal">Adding Nodes</a>
  - <a href="adding_nodes.html#node-attributes" class="reference internal">Node attributes</a>
  - <a href="adding_nodes.html#folder-and-variable-nodes" class="reference internal">Folder and Variable nodes</a>
    - <a href="adding_nodes.html#ua.newFolderParams" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newFolderParams()</code></span></a>
    - <a href="adding_nodes.html#ua.newVariableParams" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newVariableParams()</code></span></a>
  - <a href="adding_nodes.html#adding-nodes-example" class="reference internal">Adding nodes example</a>
  - <a href="adding_nodes.html#new-node-common-attributes" class="reference internal">New node common attributes</a>
  - <a href="adding_nodes.html#object-nodeattributes" class="reference internal">Object NodeAttributes</a>
    - <a href="adding_nodes.html#object-nodeattributes-example" class="reference internal">Object NodeAttributes example</a>
- <a href="http_client.html" class="reference internal">HTTP Client</a>
  - <a href="http_client.html#http-examples" class="reference internal">HTTP Examples</a>
    - <a href="http_client.html#binary-encoding" class="reference internal">Binary Encoding</a>
    - <a href="http_client.html#json-encoding" class="reference internal">JSON Encoding</a>
- <a href="init.html" class="reference internal">OPC UA Project Code Generator</a>
  - <a href="init.html#initializing-a-client" class="reference internal">Initializing a client</a>
- <a href="client_api.html" class="reference internal">Client API</a>
  - <a href="client_api.html#constructor" class="reference internal">Constructor</a>
    - <a href="client_api.html#ua.newClient" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newClient()</code></span></a>
    - <a href="client_api.html#client-configuration-table" class="reference internal">Client configuration table</a>
  - <a href="client_api.html#client-connect" class="reference internal">client:connect</a>
  - <a href="client_api.html#client-opensecurechannel" class="reference internal">client:openSecureChannel</a>
  - <a href="client_api.html#client-findservers" class="reference internal">client:findServers</a>
  - <a href="client_api.html#client-getendpoints" class="reference internal">client:getEndpoints</a>
  - <a href="client_api.html#client-createsession" class="reference internal">client:createSession</a>
  - <a href="client_api.html#client-activatesession" class="reference internal">client:activateSession</a>
  - <a href="client_api.html#client-browse" class="reference internal">client:browse</a>
  - <a href="client_api.html#client-read" class="reference internal">client:read</a>
  - <a href="client_api.html#client-write" class="reference internal">client:write</a>
  - <a href="client_api.html#client-addnodes" class="reference internal">client:addNodes</a>
  - <a href="client_api.html#client-translatebrowsepaths" class="reference internal">client:translateBrowsePaths</a>
  - <a href="client_api.html#client-createsubscription" class="reference internal">client:createSubscription</a>
  - <a href="client_api.html#client-call" class="reference internal">client:call</a>
  - <a href="client_api.html#client-renewsecurechannel" class="reference internal">client:renewSecureChannel</a>
  - <a href="client_api.html#client-checksecurechannel" class="reference internal">client:checkSecureChannel</a>
  - <a href="client_api.html#client-connected" class="reference internal">client:connected</a>
  - <a href="client_api.html#client-closesession" class="reference internal">client:closeSession</a>
  - <a href="client_api.html#client-closesecurechannel" class="reference internal">client:closeSecureChannel</a>
  - <a href="client_api.html#client-disconnect" class="reference internal">client:disconnect</a>
  - <a href="client_api.html#message-callback" class="reference internal">Message callback</a>
    - <a href="client_api.html#msgCallback" class="reference internal"><span class="pre"><code class="docutils literal notranslate">msgCallback()</code></span></a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="../model/import_export.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Import/Export"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="getting_started.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Getting Started">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/client/init.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="index.html" class="reference internal">Client</a>
  - <a href="getting_started.html" class="reference internal">Getting Started</a>
  - <a href="authentication.html" class="reference internal">Client Authentication</a>
  - <a href="browsing.html" class="reference internal">Address Space Browsing</a>
  - <a href="read_write_data.html" class="reference internal">Reading and Writing Data</a>
  - <a href="adding_nodes.html" class="reference internal">Adding Nodes</a>
  - <a href="http_client.html" class="reference internal">HTTP Client</a>
  - <a href="#" class="current reference internal">OPC UA Project Code Generator</a>
    - <a href="#initializing-a-client" class="reference internal">Initializing a client</a>
  - <a href="client_api.html" class="reference internal">Client API</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Client](index.html)
- OPC UA Project Code Generator
- <a href="../_sources/client/init.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="opc-ua-project-code-generator" class="section">

# OPC UA Project Code Generator<a href="#opc-ua-project-code-generator" class="headerlink" title="Link to this heading"></a>

To help you with the initial configuration of your OPC UA client, you can use a script that will perform the initialization of a sample application with all required files. Initial configuration includes:

- Full configuration file

- Generate self-signed certificates for all secure policies

- Lua script for starting OPC UA client

<div id="initializing-a-client" class="section">

## Initializing a client<a href="#initializing-a-client" class="headerlink" title="Link to this heading"></a>

Create a Lua file with the following content:

<div class="highlight-lua notranslate">

<div class="highlight">

    local hostname = 'localhost'
    local applicationName = 'RealTimeLogic OPCUA Client'
    local applicationUri = 'urn:realtimelogic.com:opcua:client'

    local initClient = require('opcua.init').initializeClient
    initClient(hostname, applicationName, applicationUri)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/init_client.lua" class="reference external">Full source</a>

After running the script, you will see the following files:  
- config.lua - configuration file for OPC UA client

- basic128rsa15.key - private key for security policy Basic128rsa15

- basic128rsa15.pem - self-signed certificate for security policy Basic128rsa15

- main.lua - Lua script that contains a sample of the OPC UA client application.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="http_client.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="HTTP Client"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="client_api.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Client API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/client/read_write_data.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="index.html" class="reference internal">Client</a>
  - <a href="getting_started.html" class="reference internal">Getting Started</a>
  - <a href="authentication.html" class="reference internal">Client Authentication</a>
  - <a href="browsing.html" class="reference internal">Address Space Browsing</a>
  - <a href="#" class="current reference internal">Reading and Writing Data</a>
    - <a href="#reading-attributes" class="reference internal">Reading Attributes</a>
      - <a href="#read-parameters" class="reference internal">Read parameters</a>
      - <a href="#read-attributes-of-one-node" class="reference internal">Read Attributes Of One Node</a>
      - <a href="#read-attributes-of-several-nodes" class="reference internal">Read Attributes Of Several Nodes</a>
      - <a href="#read-variables-values" class="reference internal">Read Variables Values</a>
    - <a href="#writing-attributes" class="reference internal">Writing Attributes</a>
      - <a href="#write-parameters" class="reference internal">Write Parameters</a>
      - <a href="#write-example" class="reference internal">Write example</a>
    - <a href="#node-attribute" class="reference internal">Node Attribute</a>
      - <a href="#base-node-attribute-numbers" class="reference internal">Base node attribute numbers</a>
      - <a href="#object-node-attribute-numbers" class="reference internal">Object node attribute numbers</a>
      - <a href="#object-type-node-attribute-numbers" class="reference internal">Object Type node attribute numbers</a>
      - <a href="#variable-node-attribute-numbers" class="reference internal">Variable node attribute numbers</a>
      - <a href="#variable-type-node-attribute-numbers" class="reference internal">Variable Type node attribute numbers</a>
      - <a href="#reference-type-node-attribute-numbers" class="reference internal">Reference Type node attribute numbers</a>
      - <a href="#data-type-node-attribute-numbers" class="reference internal">Data Type node attribute numbers</a>
      - <a href="#method-node-attribute-numbers" class="reference internal">Method node attribute numbers</a>
      - <a href="#view-node-attribute-numbers" class="reference internal">View node attribute numbers</a>
      - <a href="#list-of-all-attribute-numbers" class="reference internal">List of all attribute numbers</a>
  - <a href="adding_nodes.html" class="reference internal">Adding Nodes</a>
  - <a href="http_client.html" class="reference internal">HTTP Client</a>
  - <a href="init.html" class="reference internal">OPC UA Project Code Generator</a>
  - <a href="client_api.html" class="reference internal">Client API</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Client](index.html)
- Reading and Writing Data
- <a href="../_sources/client/read_write_data.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="reading-and-writing-data" class="section">


# Reading and Writing Data<a href="#reading-and-writing-data" class="headerlink" title="Link to this heading"></a>

<div id="reading-attributes" class="section">


## Reading Attributes<a href="#reading-attributes" class="headerlink" title="Link to this heading"></a>

Reading attributes is done with the *read* method. This method exists for both the <a href="client_api.html#client-read" class="reference internal"><span class="std std-ref">client</span></a> and the <a href="../server/server_api.html#server-read" class="reference internal"><span class="std std-ref">server</span></a>.

<div id="read-parameters" class="section">

### Read parameters<a href="#read-parameters" class="headerlink" title="Link to this heading"></a>

NodeId \| NodeID\[\]<span class="colon">:</span>  
(<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>) A single identifier of a node or an array of node identifiers. It will try to read <a href="#all-attributes" class="reference internal"><span class="std std-ref">all attributes</span></a> of a node. For existing attributes, a value will be returned. For absent attributes of a node, status code BadAttributeIdInvalid will be returned.

NodesToRead\[\]<span class="colon">:</span>  
An array of tables with the following fields:

**NodeID** (<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>) The identifier of a node

**AttributeId** (uint32) <a href="#attribute" class="reference internal"><span class="std std-ref">Node Attribute</span></a> Attribute of node to read

With these parameters, you can read exact attributes of a node, for example, the value of a variable or values of several variable nodes.

</div>

<div id="read-attributes-of-one-node" class="section">

### Read Attributes Of One Node<a href="#read-attributes-of-one-node" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    local ObjectsFolder = "i=85"
    local TypesFolder = "i=86"

    -- Read all possible attributes of the any node
    -- For part of attributes will be returned a valus
    -- and for part of attributes will be returned a status code BadAttributeIdInvalid
    resp,err = client:read(ObjectsFolder)
    for i,result in ipairs(resp.Results) do
      if result.StatusCode == 0 then
        ua.printTable("result", result.Value)
      else
        trace(string.format("Read attributes error: 0x%X", result.StatusCode))
      end
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_read.lua" class="reference external">Full source</a>

</div>

<div id="read-attributes-of-several-nodes" class="section">

### Read Attributes Of Several Nodes<a href="#read-attributes-of-several-nodes" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    local ObjectsFolder = "i=85"
    local TypesFolder = "i=86"

    -- Read all possible attributes of several nodes
    -- For part of attributes will be returned a valus
    -- and for part of attributes will be returned a status code BadAttributeIdInvalid
    resp,err = client:read({ObjectsFolder, TypesFolder})
    for i,result in ipairs(resp.Results) do
      if result.StatusCode == 0 then
        ua.printTable("result", result.Value)
      else
        trace(string.format("Read value error: 0x%X", result.StatusCode))
      end
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_read.lua" class="reference external">Full source</a>

</div>

<div id="read-variables-values" class="section">

### Read Variables Values<a href="#read-variables-values" class="headerlink" title="Link to this heading"></a>

Variables in OPC UA hold valuable data in the attribute Value. It is possible to read several values at once.

<div class="highlight-lua notranslate">

<div class="highlight">

    local Server_ServerStatus_BuildInfo_SoftwareVersion = "i=2260"

    local readParams = {
      NodesToRead = {
        {
          NodeId = Server_ServerStatus_CurrentTime,
          AttributeId = ua.AttributeId.Value
        },
        {
          NodeId = Server_ServerStatus_BuildInfo_SoftwareVersion,
          AttributeId = ua.AttributeId.Value
        },
      }
    }

    resp,err = client:read(readParams)
    for i,result in ipairs(resp.Results) do
      if result.StatusCode == 0 then
        ua.printTable("result", result.Value)
      else
        trace(string.format("Read value '%s' error: 0x%X", nodes.NodesToRead[i].NodeId, result.StatusCode))
      end
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_read.lua" class="reference external">Full source</a>

</div>

</div>

<div id="writing-attributes" class="section">

## Writing Attributes<a href="#writing-attributes" class="headerlink" title="Link to this heading"></a>

Changing the values of attributes is done with the Write method.

<div id="write-parameters" class="section">

### Write Parameters<a href="#write-parameters" class="headerlink" title="Link to this heading"></a>

|                  |                 |                                                                                                                   |                                                                                                     |
|------------------|-----------------|-------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------|
| **NodesToWrite** |                 | Array of nodes and attributes to write                                                                            |                                                                                                     |
|                  | **NodeId**      | <a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>       | The identifier of a node                                                                            |
|                  | **AttributeId** | uint32                                                                                                            | <a href="#attribute" class="reference internal"><span class="std std-ref">Node Attribute</span></a> |
|                  | **Value**       | <a href="../types.html#data-value-type" class="reference internal"><span class="std std-ref">DataValue</span></a> | New value of the attribute                                                                          |

</div>

<div id="write-example" class="section">

### Write example<a href="#write-example" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Update the OPC-UA server's start time.
    local nodes = {
      NodesToWrite = {
        {
          NodeId = Server_ServerStatus_StartTime,
          AttributeId = ua. AttributeId.Value,
          Value = {   -- DataValue
            Type = ua.VariantType.DateTime,
            Value = 0.0,
            StatusCode = ua.StatusCode.Good
          }
        }
      }
    }

    local resp,err = client:write(nodes)
    if resp.Results[1] ~= 0 then
      trace(string.format("Changing attribute value failed: 0x%X", resp.Results[1]))
    else
      trace(string.format("Attribute value changed sucessfully"))
    end

    client:disconnect()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_write.lua" class="reference external">Full source</a>

</div>

</div>

<div id="node-attribute" class="section">


## Node Attribute<a href="#node-attribute" class="headerlink" title="Link to this heading"></a>

Every node in the address space is represented by a set of attributes.

<div id="base-node-attribute-numbers" class="section">

### Base node attribute numbers<a href="#base-node-attribute-numbers" class="headerlink" title="Link to this heading"></a>

|        |                     |                 |
|--------|---------------------|-----------------|
| Number | Attribute name      | Data type       |
| 1      | NodeId              | nodeID          |
| 2      | NodeClass           | uint32          |
| 3      | BrowseName          | qualifiedName   |
| 4      | DisplayName         | localizedText   |
| 5      | Description         | localizedText   |
| 6      | WriteMask           | uint32          |
| 7      | UserWriteMask       | uint32          |
| 24     | RolePermissions     | structure (TBD) |
| 25     | UserRolePermissions | structure (TBD) |
| 26     | AccessRestrictions  | uint16          |

</div>

<div id="object-node-attribute-numbers" class="section">

### Object node attribute numbers<a href="#object-node-attribute-numbers" class="headerlink" title="Link to this heading"></a>

<table class="docutils align-default">
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<tbody>
<tr class="odd row-odd">
<td><p>Number</p></td>
<td><p>Attribute name</p></td>
<td><p>Data type</p></td>
</tr>
<tr class="even row-even">
<td><ul>
<li></li>
</ul></td>
<td><p>Base attributes</p></td>
<td></td>
</tr>
<tr class="odd row-odd">
<td><p>12</p></td>
<td><p>EventNotifier</p></td>
<td><p>byte</p></td>
</tr>
</tbody>
</table>

</div>

<div id="object-type-node-attribute-numbers" class="section">

### Object Type node attribute numbers<a href="#object-type-node-attribute-numbers" class="headerlink" title="Link to this heading"></a>

<table class="docutils align-default">
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<tbody>
<tr class="odd row-odd">
<td><p>Number</p></td>
<td><p>Attribute name</p></td>
<td><p>Data type</p></td>
</tr>
<tr class="even row-even">
<td><ul>
<li></li>
</ul></td>
<td><p>Base attributes</p></td>
<td></td>
</tr>
<tr class="odd row-odd">
<td><p>8</p></td>
<td><p>IsAbstract</p></td>
<td><p>boolean</p></td>
</tr>
</tbody>
</table>

</div>

<div id="variable-node-attribute-numbers" class="section">

### Variable node attribute numbers<a href="#variable-node-attribute-numbers" class="headerlink" title="Link to this heading"></a>

<table class="docutils align-default">
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<tbody>
<tr class="odd row-odd">
<td><p>Number</p></td>
<td><p>Attribute name</p></td>
<td><p>Data type</p></td>
</tr>
<tr class="even row-even">
<td><ul>
<li></li>
</ul></td>
<td><p>Base attributes</p></td>
<td></td>
</tr>
<tr class="odd row-odd">
<td><p>13</p></td>
<td><p>Value</p></td>
<td><p>DataValue</p></td>
</tr>
<tr class="even row-even">
<td><p>14</p></td>
<td><p>DataType</p></td>
<td><p>NodeID</p></td>
</tr>
<tr class="odd row-odd">
<td><p>15</p></td>
<td><p>Rank</p></td>
<td><p>int32</p></td>
</tr>
<tr class="even row-even">
<td><p>16</p></td>
<td><p>ArrayDimensions</p></td>
<td><p>int32[]</p></td>
</tr>
<tr class="odd row-odd">
<td><p>17</p></td>
<td><p>AccessLevel</p></td>
<td><p>byte</p></td>
</tr>
<tr class="even row-even">
<td><p>18</p></td>
<td><p>UserAccessLevel</p></td>
<td><p>byte</p></td>
</tr>
<tr class="odd row-odd">
<td><p>19</p></td>
<td><p>MinimumSamplingInterval</p></td>
<td><p>double</p></td>
</tr>
<tr class="even row-even">
<td><p>20</p></td>
<td><p>Historizing</p></td>
<td><p>boolean</p></td>
</tr>
<tr class="odd row-odd">
<td><p>27</p></td>
<td><p>AccessLevelEx</p></td>
<td><p>uint32</p></td>
</tr>
</tbody>
</table>

</div>

<div id="variable-type-node-attribute-numbers" class="section">

### Variable Type node attribute numbers<a href="#variable-type-node-attribute-numbers" class="headerlink" title="Link to this heading"></a>

<table class="docutils align-default">
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<tbody>
<tr class="odd row-odd">
<td><p>Number</p></td>
<td><p>Attribute name</p></td>
<td><p>Data type</p></td>
</tr>
<tr class="even row-even">
<td><ul>
<li></li>
</ul></td>
<td><p>Base attributes</p></td>
<td></td>
</tr>
<tr class="odd row-odd">
<td><p>8</p></td>
<td><p>IsAbstract</p></td>
<td><p>boolean</p></td>
</tr>
<tr class="even row-even">
<td><p>13</p></td>
<td><p>Value</p></td>
<td><p>DataValue</p></td>
</tr>
<tr class="odd row-odd">
<td><p>14</p></td>
<td><p>DataType</p></td>
<td><p>NodeID</p></td>
</tr>
<tr class="even row-even">
<td><p>15</p></td>
<td><p>Rank</p></td>
<td><p>int32</p></td>
</tr>
<tr class="odd row-odd">
<td><p>16</p></td>
<td><p>ArrayDimensions</p></td>
<td><p>int32[]</p></td>
</tr>
</tbody>
</table>

</div>

<div id="reference-type-node-attribute-numbers" class="section">

### Reference Type node attribute numbers<a href="#reference-type-node-attribute-numbers" class="headerlink" title="Link to this heading"></a>

<table class="docutils align-default">
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<tbody>
<tr class="odd row-odd">
<td><p>Number</p></td>
<td><p>Attribute name</p></td>
<td><p>Data type</p></td>
</tr>
<tr class="even row-even">
<td><ul>
<li></li>
</ul></td>
<td><p>Base attributes</p></td>
<td></td>
</tr>
<tr class="odd row-odd">
<td><p>8</p></td>
<td><p>IsAbstract</p></td>
<td><p>boolean</p></td>
</tr>
<tr class="even row-even">
<td><p>9</p></td>
<td><p>Symmetric</p></td>
<td><p>boolean</p></td>
</tr>
<tr class="odd row-odd">
<td><p>10</p></td>
<td><p>InverseName</p></td>
<td><p>localizedText</p></td>
</tr>
</tbody>
</table>

</div>

<div id="data-type-node-attribute-numbers" class="section">

### Data Type node attribute numbers<a href="#data-type-node-attribute-numbers" class="headerlink" title="Link to this heading"></a>

<table class="docutils align-default">
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<tbody>
<tr class="odd row-odd">
<td><p>Number</p></td>
<td><p>Attribute name</p></td>
<td><p>Data type</p></td>
</tr>
<tr class="even row-even">
<td><ul>
<li></li>
</ul></td>
<td><p>Base attributes</p></td>
<td></td>
</tr>
<tr class="odd row-odd">
<td><p>8</p></td>
<td><p>IsAbstract</p></td>
<td><p>boolean</p></td>
</tr>
<tr class="even row-even">
<td><p>23</p></td>
<td><p>DataTypeDefinition</p></td>
<td><p>nodeID</p></td>
</tr>
</tbody>
</table>

</div>

<div id="method-node-attribute-numbers" class="section">

### Method node attribute numbers<a href="#method-node-attribute-numbers" class="headerlink" title="Link to this heading"></a>

<table class="docutils align-default">
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<tbody>
<tr class="odd row-odd">
<td><p>Number</p></td>
<td><p>Attribute name</p></td>
<td><p>Data type</p></td>
</tr>
<tr class="even row-even">
<td><ul>
<li></li>
</ul></td>
<td><p>Base attributes</p></td>
<td></td>
</tr>
<tr class="odd row-odd">
<td><p>21</p></td>
<td><p>Executable</p></td>
<td><p>boolean</p></td>
</tr>
<tr class="even row-even">
<td><p>22</p></td>
<td><p>UserExecutable</p></td>
<td><p>boolean</p></td>
</tr>
</tbody>
</table>

</div>

<div id="view-node-attribute-numbers" class="section">

### View node attribute numbers<a href="#view-node-attribute-numbers" class="headerlink" title="Link to this heading"></a>

<table class="docutils align-default">
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<tbody>
<tr class="odd row-odd">
<td><p>Number</p></td>
<td><p>Attribute name</p></td>
<td><p>Data type</p></td>
</tr>
<tr class="even row-even">
<td><ul>
<li></li>
</ul></td>
<td><p>Base attributes</p></td>
<td></td>
</tr>
<tr class="odd row-odd">
<td><p>11</p></td>
<td><p>ContainsNoLoops</p></td>
<td><p>boolean</p></td>
</tr>
<tr class="even row-even">
<td><p>12</p></td>
<td><p>EventNotifier</p></td>
<td><p>byte</p></td>
</tr>
</tbody>
</table>

</div>

<div id="list-of-all-attribute-numbers" class="section">


### List of all attribute numbers<a href="#list-of-all-attribute-numbers" class="headerlink" title="Link to this heading"></a>

|        |                         |                 |
|--------|-------------------------|-----------------|
| Number | Attribute name          | Data type       |
| 1      | NodeId                  | nodeID          |
| 2      | NodeClass               | uint32          |
| 3      | BrowseName              | qualifiedName   |
| 4      | DisplayName             | localizedText   |
| 5      | Description             | localizedText   |
| 6      | WriteMask               | uint32          |
| 7      | UserWriteMask           | uint32          |
| 8      | IsAbstract              | boolean         |
| 9      | Symmetric               | boolean         |
| 10     | InverseName             | localizedText   |
| 11     | ContainsNoLoops         | boolean         |
| 12     | EventNotifier           | byte            |
| 13     | Value                   | DataValue       |
| 14     | DataType                | NodeID          |
| 15     | Rank                    | int32           |
| 16     | ArrayDimensions         | int32\[\]       |
| 17     | AccessLevel             | byte            |
| 18     | UserAccessLevel         | byte            |
| 19     | MinimumSamplingInterval | double          |
| 20     | Historizing             | boolean         |
| 21     | Executable              | boolean         |
| 22     | UserExecutable          | boolean         |
| 23     | DataTypeDefinition      | nodeID          |
| 24     | RolePermissions         | structure (TBD) |
| 25     | UserRolePermissions     | structure (TBD) |
| 26     | AccessRestrictions      | uint16          |
| 27     | AccessLevelEx           | uint32          |

</div>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="browsing.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Address Space Browsing"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="adding_nodes.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Adding Nodes">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/genindex.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="about.html" class="reference internal">About opcua-lua</a>
- <a href="opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="model/index.html" class="reference internal">Address Space API</a>
- <a href="client/index.html" class="reference internal">Client</a>
- <a href="server/index.html" class="reference internal">Server</a>
- <a href="pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="integrations/index.html" class="reference internal">Examples</a>
- <a href="logging.html" class="reference internal">Logging subsystem</a>
- <a href="secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="types.html" class="reference internal">Data Types</a>
- <a href="interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="index.html" class="icon icon-home" aria-label="Home"></a>
- Index
- 

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

# Index

<div class="genindex-jumpbox">

[**B**](#B) \| [**C**](#C) \| [**M**](#M) \| [**U**](#U) \| [**W**](#W)

</div>

## B

<table class="indextable genindextable" style="width: 100%">
<colgroup>
<col style="width: 100%" />
</colgroup>
<tbody>
<tr class="odd">
<td style="width: 33%; vertical-align: top"><ul>
<li>built-in function
<ul>
<li><a href="server/data_source.html#callback">callback()</a></li>
<li><a href="pubsub/mqtt.html#messageCallback">messageCallback()</a></li>
<li><a href="client/client_api.html#msgCallback">msgCallback()</a></li>
<li><a href="model/model.html#ua.baseModel">ua.baseModel()</a></li>
<li><a href="model/model.html#ua.emptyModel">ua.emptyModel()</a></li>
<li><a href="client/client_api.html#ua.newClient">ua.newClient()</a></li>
<li><a href="client/adding_nodes.html#ua.newFolderParams">ua.newFolderParams()</a></li>
<li><a href="pubsub/mqtt.html#ua.newMqttClient">ua.newMqttClient()</a></li>
<li><a href="server/server_api.html#ua.newServer">ua.newServer()</a></li>
<li><a href="client/adding_nodes.html#ua.newVariableParams">ua.newVariableParams()</a></li>
<li><a href="server/server_api.html#writeHook">writeHook()</a></li>
</ul></li>
</ul></td>
</tr>
</tbody>
</table>

## C

<table class="indextable genindextable" style="width: 100%">
<colgroup>
<col style="width: 100%" />
</colgroup>
<tbody>
<tr class="odd">
<td style="width: 33%; vertical-align: top"><ul>
<li>callback()
<ul>
<li><a href="server/data_source.html#callback">built-in function</a></li>
</ul></li>
</ul></td>
</tr>
</tbody>
</table>

## M

<table class="indextable genindextable" style="width: 100%">
<colgroup>
<col style="width: 50%" />
<col style="width: 50%" />
</colgroup>
<tbody>
<tr class="odd">
<td style="width: 33%; vertical-align: top"><ul>
<li>messageCallback()
<ul>
<li><a href="pubsub/mqtt.html#messageCallback">built-in function</a></li>
</ul></li>
</ul></td>
<td style="width: 33%; vertical-align: top"><ul>
<li>msgCallback()
<ul>
<li><a href="client/client_api.html#msgCallback">built-in function</a></li>
</ul></li>
</ul></td>
</tr>
</tbody>
</table>

## U

<table class="indextable genindextable" style="width: 100%">
<colgroup>
<col style="width: 50%" />
<col style="width: 50%" />
</colgroup>
<tbody>
<tr class="odd">
<td style="width: 33%; vertical-align: top"><ul>
<li>ua.baseModel()
<ul>
<li><a href="model/model.html#ua.baseModel">built-in function</a></li>
</ul></li>
<li>ua.emptyModel()
<ul>
<li><a href="model/model.html#ua.emptyModel">built-in function</a></li>
</ul></li>
<li>ua.newClient()
<ul>
<li><a href="client/client_api.html#ua.newClient">built-in function</a></li>
</ul></li>
<li>ua.newFolderParams()
<ul>
<li><a href="client/adding_nodes.html#ua.newFolderParams">built-in function</a></li>
</ul></li>
</ul></td>
<td style="width: 33%; vertical-align: top"><ul>
<li>ua.newMqttClient()
<ul>
<li><a href="pubsub/mqtt.html#ua.newMqttClient">built-in function</a></li>
</ul></li>
<li>ua.newServer()
<ul>
<li><a href="server/server_api.html#ua.newServer">built-in function</a></li>
</ul></li>
<li>ua.newVariableParams()
<ul>
<li><a href="client/adding_nodes.html#ua.newVariableParams">built-in function</a></li>
</ul></li>
</ul></td>
</tr>
</tbody>
</table>

## W

<table class="indextable genindextable" style="width: 100%">
<colgroup>
<col style="width: 100%" />
</colgroup>
<tbody>
<tr class="odd">
<td style="width: 33%; vertical-align: top"><ul>
<li>writeHook()
<ul>
<li><a href="server/server_api.html#writeHook">built-in function</a></li>
</ul></li>
</ul></td>
</tr>
</tbody>
</table>

</div>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/index.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="#" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="about.html" class="reference internal">About opcua-lua</a>
- <a href="opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="model/index.html" class="reference internal">Address Space API</a>
- <a href="client/index.html" class="reference internal">Client</a>
- <a href="server/index.html" class="reference internal">Server</a>
- <a href="pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="integrations/index.html" class="reference internal">Examples</a>
- <a href="logging.html" class="reference internal">Logging subsystem</a>
- <a href="secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="types.html" class="reference internal">Data Types</a>
- <a href="interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](#)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="#" class="icon icon-home" aria-label="Home"></a>
- OPC UA Client and Server
- <a href="_sources/index.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="opc-ua-client-and-server" class="section">

# OPC UA Client and Server<a href="#opc-ua-client-and-server" class="headerlink" title="Link to this heading"></a>

This documentation covers Real Time Logic’s Lua OPC UA stack, including the address-space model, Client/Server APIs, PubSub over MQTT, security, and ready-to-run examples.

<div id="where-to-start" class="section">

## Where to start<a href="#where-to-start" class="headerlink" title="Link to this heading"></a>

- New to this Lua stack: <a href="about.html" class="reference internal"><span class="doc">About opcua-lua</span></a>

- New to OPC UA: <a href="opcua_overview.html" class="reference internal"><span class="doc">OPC UA overview</span></a>

- Learning the Lua APIs step by step: <a href="integrations/learning_examples.html" class="reference internal"><span class="doc">Learning OPC UA with Mako Server</span></a>

- Working with nodes, types, and models: <a href="model/index.html" class="reference internal"><span class="doc">Address Space API</span></a>

- Building an OPC UA client: <a href="client/index.html" class="reference internal"><span class="doc">Client</span></a>

- Building an OPC UA server: <a href="server/index.html" class="reference internal"><span class="doc">Server</span></a>

- Using MQTT PubSub: <a href="pubsub/index.html" class="reference internal"><span class="doc">Publish Subscribe API</span></a>

- Looking for ready-to-run examples: <a href="integrations/index.html" class="reference internal"><span class="doc">Examples</span></a>

<div class="toctree-wrapper compound">

<span class="caption-text">Documentation</span>

- <a href="about.html" class="reference internal">About opcua-lua</a>
  - <a href="about.html#why-lua" class="reference internal">Why Lua</a>
  - <a href="about.html#where-it-fits" class="reference internal">Where it fits</a>
  - <a href="about.html#what-to-read-next" class="reference internal">What to read next</a>
- <a href="opcua_overview.html" class="reference internal">OPC UA overview</a>
  - <a href="opcua_overview.html#the-address-space" class="reference internal">The address space</a>
  - <a href="opcua_overview.html#types-and-instances" class="reference internal">Types and instances</a>
  - <a href="opcua_overview.html#common-client-server-services" class="reference internal">Common Client/Server services</a>
  - <a href="opcua_overview.html#why-it-feels-complex" class="reference internal">Why it feels complex</a>
  - <a href="opcua_overview.html#practical-mental-model" class="reference internal">Practical mental model</a>
- <a href="model/index.html" class="reference internal">Address Space API</a>
  - <a href="model/model.html" class="reference internal">Model methods</a>
    - <a href="model/model.html#ua.emptyModel" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.emptyModel()</code></span></a>
    - <a href="model/model.html#ua.baseModel" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.baseModel()</code></span></a>
  - <a href="model/browser.html" class="reference internal">Browser</a>
  - <a href="model/editor.html" class="reference internal">Editor</a>
    - <a href="model/editor.html#object" class="reference internal">Object</a>
    - <a href="model/editor.html#objecttype" class="reference internal">ObjectType</a>
    - <a href="model/editor.html#variable" class="reference internal">Variable</a>
    - <a href="model/editor.html#variabletype" class="reference internal">VariableType</a>
    - <a href="model/editor.html#enum" class="reference internal">Enum</a>
    - <a href="model/editor.html#structure" class="reference internal">Structure</a>
    - <a href="model/editor.html#method" class="reference internal">Method</a>
  - <a href="model/import_export.html" class="reference internal">Import/Export</a>
- <a href="client/index.html" class="reference internal">Client</a>
  - <a href="client/getting_started.html" class="reference internal">Getting Started</a>
    - <a href="client/getting_started.html#connecting-to-server" class="reference internal">Connecting to Server</a>
    - <a href="client/getting_started.html#browsing-address-space" class="reference internal">Browsing Address Space</a>
    - <a href="client/getting_started.html#reading-node-attributes" class="reference internal">Reading Node Attributes</a>
    - <a href="client/getting_started.html#closing-server-connection" class="reference internal">Closing Server Connection</a>
    - <a href="client/getting_started.html#asynchronous-cosocket-mode" class="reference internal">Asynchronous Cosocket Mode</a>
  - <a href="client/authentication.html" class="reference internal">Client Authentication</a>
    - <a href="client/authentication.html#working-with-client-sessions" class="reference internal">Working with client sessions</a>
    - <a href="client/authentication.html#anonymous" class="reference internal">Anonymous</a>
    - <a href="client/authentication.html#user-name" class="reference internal">User Name</a>
    - <a href="client/authentication.html#x509-certificate" class="reference internal">x509 certificate</a>
    - <a href="client/authentication.html#jwt-oauth2-azure" class="reference internal">JWT, OAuth2, Azure</a>
  - <a href="client/browsing.html" class="reference internal">Address Space Browsing</a>
    - <a href="client/browsing.html#browsing-by-string-nodeid" class="reference internal">Browsing by String NodeID</a>
    - <a href="client/browsing.html#detailed-parameters" class="reference internal">Detailed Parameters</a>
  - <a href="client/read_write_data.html" class="reference internal">Reading and Writing Data</a>
    - <a href="client/read_write_data.html#reading-attributes" class="reference internal">Reading Attributes</a>
      - <a href="client/read_write_data.html#read-parameters" class="reference internal">Read parameters</a>
      - <a href="client/read_write_data.html#read-attributes-of-one-node" class="reference internal">Read Attributes Of One Node</a>
      - <a href="client/read_write_data.html#read-attributes-of-several-nodes" class="reference internal">Read Attributes Of Several Nodes</a>
      - <a href="client/read_write_data.html#read-variables-values" class="reference internal">Read Variables Values</a>
    - <a href="client/read_write_data.html#writing-attributes" class="reference internal">Writing Attributes</a>
      - <a href="client/read_write_data.html#write-parameters" class="reference internal">Write Parameters</a>
      - <a href="client/read_write_data.html#write-example" class="reference internal">Write example</a>
    - <a href="client/read_write_data.html#node-attribute" class="reference internal">Node Attribute</a>
      - <a href="client/read_write_data.html#base-node-attribute-numbers" class="reference internal">Base node attribute numbers</a>
      - <a href="client/read_write_data.html#object-node-attribute-numbers" class="reference internal">Object node attribute numbers</a>
      - <a href="client/read_write_data.html#object-type-node-attribute-numbers" class="reference internal">Object Type node attribute numbers</a>
      - <a href="client/read_write_data.html#variable-node-attribute-numbers" class="reference internal">Variable node attribute numbers</a>
      - <a href="client/read_write_data.html#variable-type-node-attribute-numbers" class="reference internal">Variable Type node attribute numbers</a>
      - <a href="client/read_write_data.html#reference-type-node-attribute-numbers" class="reference internal">Reference Type node attribute numbers</a>
      - <a href="client/read_write_data.html#data-type-node-attribute-numbers" class="reference internal">Data Type node attribute numbers</a>
      - <a href="client/read_write_data.html#method-node-attribute-numbers" class="reference internal">Method node attribute numbers</a>
      - <a href="client/read_write_data.html#view-node-attribute-numbers" class="reference internal">View node attribute numbers</a>
      - <a href="client/read_write_data.html#list-of-all-attribute-numbers" class="reference internal">List of all attribute numbers</a>
  - <a href="client/adding_nodes.html" class="reference internal">Adding Nodes</a>
    - <a href="client/adding_nodes.html#node-attributes" class="reference internal">Node attributes</a>
    - <a href="client/adding_nodes.html#folder-and-variable-nodes" class="reference internal">Folder and Variable nodes</a>
      - <a href="client/adding_nodes.html#ua.newFolderParams" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newFolderParams()</code></span></a>
      - <a href="client/adding_nodes.html#ua.newVariableParams" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newVariableParams()</code></span></a>
    - <a href="client/adding_nodes.html#adding-nodes-example" class="reference internal">Adding nodes example</a>
    - <a href="client/adding_nodes.html#new-node-common-attributes" class="reference internal">New node common attributes</a>
    - <a href="client/adding_nodes.html#object-nodeattributes" class="reference internal">Object NodeAttributes</a>
      - <a href="client/adding_nodes.html#object-nodeattributes-example" class="reference internal">Object NodeAttributes example</a>
  - <a href="client/http_client.html" class="reference internal">HTTP Client</a>
    - <a href="client/http_client.html#http-examples" class="reference internal">HTTP Examples</a>
      - <a href="client/http_client.html#binary-encoding" class="reference internal">Binary Encoding</a>
      - <a href="client/http_client.html#json-encoding" class="reference internal">JSON Encoding</a>
  - <a href="client/init.html" class="reference internal">OPC UA Project Code Generator</a>
    - <a href="client/init.html#initializing-a-client" class="reference internal">Initializing a client</a>
  - <a href="client/client_api.html" class="reference internal">Client API</a>
    - <a href="client/client_api.html#constructor" class="reference internal">Constructor</a>
      - <a href="client/client_api.html#ua.newClient" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newClient()</code></span></a>
      - <a href="client/client_api.html#client-configuration-table" class="reference internal">Client configuration table</a>
    - <a href="client/client_api.html#client-connect" class="reference internal">client:connect</a>
    - <a href="client/client_api.html#client-opensecurechannel" class="reference internal">client:openSecureChannel</a>
    - <a href="client/client_api.html#client-findservers" class="reference internal">client:findServers</a>
    - <a href="client/client_api.html#client-getendpoints" class="reference internal">client:getEndpoints</a>
    - <a href="client/client_api.html#client-createsession" class="reference internal">client:createSession</a>
    - <a href="client/client_api.html#client-activatesession" class="reference internal">client:activateSession</a>
    - <a href="client/client_api.html#client-browse" class="reference internal">client:browse</a>
    - <a href="client/client_api.html#client-read" class="reference internal">client:read</a>
    - <a href="client/client_api.html#client-write" class="reference internal">client:write</a>
    - <a href="client/client_api.html#client-addnodes" class="reference internal">client:addNodes</a>
    - <a href="client/client_api.html#client-translatebrowsepaths" class="reference internal">client:translateBrowsePaths</a>
    - <a href="client/client_api.html#client-createsubscription" class="reference internal">client:createSubscription</a>
    - <a href="client/client_api.html#client-call" class="reference internal">client:call</a>
    - <a href="client/client_api.html#client-renewsecurechannel" class="reference internal">client:renewSecureChannel</a>
    - <a href="client/client_api.html#client-checksecurechannel" class="reference internal">client:checkSecureChannel</a>
    - <a href="client/client_api.html#client-connected" class="reference internal">client:connected</a>
    - <a href="client/client_api.html#client-closesession" class="reference internal">client:closeSession</a>
    - <a href="client/client_api.html#client-closesecurechannel" class="reference internal">client:closeSecureChannel</a>
    - <a href="client/client_api.html#client-disconnect" class="reference internal">client:disconnect</a>
    - <a href="client/client_api.html#message-callback" class="reference internal">Message callback</a>
      - <a href="client/client_api.html#msgCallback" class="reference internal"><span class="pre"><code class="docutils literal notranslate">msgCallback()</code></span></a>
- <a href="server/index.html" class="reference internal">Server</a>
  - <a href="server/getting_started.html" class="reference internal">Getting Started with Server</a>
    - <a href="server/getting_started.html#creating-a-server" class="reference internal">Creating a Server</a>
    - <a href="server/getting_started.html#adding-nodes-on-server" class="reference internal">Adding Nodes on server</a>
    - <a href="server/getting_started.html#starting-the-server" class="reference internal">Starting the Server</a>
    - <a href="server/getting_started.html#stopping-the-server" class="reference internal">Stopping the Server</a>
  - <a href="server/server_config.html" class="reference internal">Configuration table</a>
  - <a href="server/data_source.html" class="reference internal">Exporting Device Data</a>
    - <a href="server/data_source.html#value-callback" class="reference internal">Value Callback</a>
      - <a href="server/data_source.html#callback" class="reference internal"><span class="pre"><code class="docutils literal notranslate">callback()</code></span></a>
    - <a href="server/data_source.html#writing-callback-parameters" class="reference internal">Writing Callback Parameters</a>
    - <a href="server/data_source.html#reading-callback-parameters" class="reference internal">Reading Callback Parameters</a>
    - <a href="server/data_source.html#error-reporting" class="reference internal">Error reporting</a>
    - <a href="server/data_source.html#custom-data-source-example" class="reference internal">Custom data source example</a>
  - <a href="server/xml_models.html" class="reference internal">XML Models</a>
    - <a href="server/xml_models.html#loading-xml-models" class="reference internal">Loading XML models</a>
    - <a href="server/xml_models.html#export-xml-models" class="reference internal">Export XML models</a>
  - <a href="server/authentication.html" class="reference internal">Server side authentication</a>
    - <a href="server/authentication.html#identity-token-policies" class="reference internal">Identity token policies</a>
    - <a href="server/authentication.html#authentication-callback" class="reference internal">Authentication callback</a>
    - <a href="server/authentication.html#anonymous-token" class="reference internal">Anonymous token</a>
    - <a href="server/authentication.html#user-name-and-password" class="reference internal">User Name and password</a>
    - <a href="server/authentication.html#x509-user-certificate" class="reference internal">X509 user certificate</a>
    - <a href="server/authentication.html#issued-tokens" class="reference internal">Issued tokens</a>
  - <a href="server/init_opcua_project.html" class="reference internal">OPC UA Server Project Code Generator</a>
    - <a href="server/init_opcua_project.html#initializing-a-server" class="reference internal">Initializing a server</a>
  - <a href="server/http.html" class="reference internal">HTTP Server</a>
    - <a href="server/http.html#example" class="reference internal">Example</a>
  - <a href="server/server_api.html" class="reference internal">Server API</a>
    - <a href="server/server_api.html#server-constructor" class="reference internal">Server Constructor</a>
      - <a href="server/server_api.html#ua.newServer" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newServer()</code></span></a>
      - <a href="server/server_api.html#server-default-configuration" class="reference internal">Server default configuration</a>
      - <a href="server/server_api.html#server-custom-configuration" class="reference internal">Server custom configuration</a>
    - <a href="server/server_api.html#server-initialize" class="reference internal">server:initialize</a>
    - <a href="server/server_api.html#server-run" class="reference internal">server:run</a>
    - <a href="server/server_api.html#server-shutdown" class="reference internal">server:shutdown</a>
    - <a href="server/server_api.html#server-addnodes" class="reference internal">server:addNodes</a>
    - <a href="server/server_api.html#server-browse" class="reference internal">server:browse</a>
    - <a href="server/server_api.html#server-read" class="reference internal">server:read</a>
    - <a href="server/server_api.html#server-write" class="reference internal">server:write</a>
    - <a href="server/server_api.html#server-setvaluecallback" class="reference internal">server:setValueCallback</a>
    - <a href="server/server_api.html#server-setwritehook" class="reference internal">server:setWriteHook</a>
      - <a href="server/server_api.html#writeHook" class="reference internal"><span class="pre"><code class="docutils literal notranslate">writeHook()</code></span></a>
    - <a href="server/server_api.html#server-loadxmlmodels" class="reference internal">server:loadXmlModels</a>
    - <a href="server/server_api.html#server-createnamespace" class="reference internal">server:createNamespace</a>
    - <a href="server/server_api.html#server-exportxmlmodels" class="reference internal">server:exportXmlModels</a>
- <a href="pubsub/index.html" class="reference internal">Publish Subscribe API</a>
  - <a href="pubsub/client_server_vs_pubsub.html" class="reference internal">Client/Server vs PubSub over MQTT</a>
    - <a href="pubsub/client_server_vs_pubsub.html#opc-ua-client-server" class="reference internal">OPC UA Client/Server</a>
    - <a href="pubsub/client_server_vs_pubsub.html#opc-ua-pubsub-over-mqtt" class="reference internal">OPC UA PubSub over MQTT</a>
    - <a href="pubsub/client_server_vs_pubsub.html#comparison" class="reference internal">Comparison</a>
    - <a href="pubsub/client_server_vs_pubsub.html#choosing-the-right-model" class="reference internal">Choosing the right model</a>
  - <a href="pubsub/mqtt.html" class="reference internal">MQTT PubSub API</a>
    - <a href="pubsub/mqtt.html#mqtt-broker-selection" class="reference internal">MQTT broker selection</a>
    - <a href="pubsub/mqtt.html#mqtt-client-constructor" class="reference internal">MQTT Client constructor</a>
      - <a href="pubsub/mqtt.html#ua.newMqttClient" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newMqttClient()</code></span></a>
    - <a href="pubsub/mqtt.html#mqtt-connect" class="reference internal">MQTT Connect</a>
    - <a href="pubsub/mqtt.html#mqtt-subscribe" class="reference internal">MQTT subscribe</a>
      - <a href="pubsub/mqtt.html#messageCallback" class="reference internal"><span class="pre"><code class="docutils literal notranslate">messageCallback()</code></span></a>
    - <a href="pubsub/mqtt.html#mqtt-publishing" class="reference internal">MQTT publishing</a>
  - <a href="pubsub/mqtt_examples.html" class="reference internal">MQTT PubSub Examples</a>
    - <a href="pubsub/mqtt_examples.html#monitoring-opc-ua-server-changes" class="reference internal">Monitoring OPC UA server changes</a>
    - <a href="pubsub/mqtt_examples.html#publishing-data-to-mqtt-broker" class="reference internal">Publishing data to MQTT broker</a>
    - <a href="pubsub/mqtt_examples.html#subscribing-to-mqtt-messages" class="reference internal">Subscribing to MQTT messages</a>
  - <a href="pubsub/mqtt_message_format.html" class="reference internal">UADP Message</a>
    - <a href="pubsub/mqtt_message_format.html#common-fields" class="reference internal">Common Fields</a>
      - <a href="pubsub/mqtt_message_format.html#groupheader" class="reference internal">GroupHeader</a>
      - <a href="pubsub/mqtt_message_format.html#securityheader" class="reference internal">SecurityHeader</a>
    - <a href="pubsub/mqtt_message_format.html#uadp-datasetmessage" class="reference internal">UADP DataSetMessage</a>
    - <a href="pubsub/mqtt_message_format.html#uadp-announcementmessage" class="reference internal">UADP AnnouncementMessage</a>
      - <a href="pubsub/mqtt_message_format.html#dataset-metadata" class="reference internal">DATASET_METADATA</a>
      - <a href="pubsub/mqtt_message_format.html#metadata" class="reference internal">Metadata</a>
      - <a href="pubsub/mqtt_message_format.html#metadata-fields" class="reference internal">Metadata Fields</a>
- <a href="integrations/index.html" class="reference internal">Examples</a>
  - <a href="integrations/learning_examples.html" class="reference internal">Learning OPC UA with Mako Server</a>
    - <a href="integrations/learning_examples.html#run-the-tutorial-examples" class="reference internal">Run the tutorial examples</a>
    - <a href="integrations/learning_examples.html#run-the-pubsub-tutorial-examples" class="reference internal">Run the PubSub tutorial examples</a>
    - <a href="integrations/learning_examples.html#minimal-server" class="reference internal">1. Minimal server</a>
    - <a href="integrations/learning_examples.html#add-a-folder" class="reference internal">2. Add a folder</a>
    - <a href="integrations/learning_examples.html#add-variables" class="reference internal">3. Add variables</a>
    - <a href="integrations/learning_examples.html#write-a-variable" class="reference internal">4. Write a variable</a>
    - <a href="integrations/learning_examples.html#use-a-value-callback" class="reference internal">5. Use a value callback</a>
    - <a href="integrations/learning_examples.html#read-with-a-local-client" class="reference internal">6. Read with a local client</a>
    - <a href="integrations/learning_examples.html#write-with-a-local-client" class="reference internal">7. Write with a local client</a>
    - <a href="integrations/learning_examples.html#call-a-method" class="reference internal">8. Call a method</a>
    - <a href="integrations/learning_examples.html#define-a-structure-type" class="reference internal">9. Define a structure type</a>
    - <a href="integrations/learning_examples.html#publish-json-with-mqtt-pubsub" class="reference internal">10. Publish JSON with MQTT PubSub</a>
    - <a href="integrations/learning_examples.html#publish-binary-uadp-with-mqtt-pubsub" class="reference internal">11. Publish binary UADP with MQTT PubSub</a>
    - <a href="integrations/learning_examples.html#publish-an-opc-ua-server-node" class="reference internal">12. Publish an OPC UA server node</a>
    - <a href="integrations/learning_examples.html#more-advanced-pubsub-examples" class="reference internal">More advanced PubSub examples</a>
    - <a href="integrations/learning_examples.html#next-steps" class="reference internal">Next steps</a>
  - <a href="integrations/thirdparty_clients.html" class="reference internal">How to Connect Third-Party Clients</a>
    - <a href="integrations/thirdparty_clients.html#id1" class="reference internal">UAExpert</a>
    - <a href="integrations/thirdparty_clients.html#prosys-opc-ua" class="reference internal">ProSys OPC UA</a>
    - <a href="integrations/thirdparty_clients.html#datafeed-opcua" class="reference internal">dataFEED OPCUA</a>
  - <a href="integrations/esp32.html" class="reference internal">ESP32 Microcontroller Examples</a>
    - <a href="integrations/esp32.html#ready-to-run-opc-ua-server-example" class="reference internal">Ready-to-run OPC UA Server Example</a>
    - <a href="integrations/esp32.html#ignition-scada" class="reference internal">Ignition SCADA</a>
      - <a href="integrations/esp32.html#add-the-esp32-opcua-server-to-ignition" class="reference internal">Add the ESP32 OPCUA server to Ignition</a>
      - <a href="integrations/esp32.html#ready-to-use-ignition-visualization-project" class="reference internal">Ready-to-use Ignition Visualization Project</a>
- <a href="logging.html" class="reference internal">Logging subsystem</a>
- <a href="secure_policy.html" class="reference internal">Secure Policy</a>
  - <a href="secure_policy.html#none" class="reference internal">None</a>
  - <a href="secure_policy.html#basic128rsa15" class="reference internal">Basic128Rsa15</a>
  - <a href="secure_policy.html#aes128-sha256-rsaoaep" class="reference internal">Aes128_Sha256_RsaOaep</a>
  - <a href="secure_policy.html#basic256sha256" class="reference internal">Basic256Sha256</a>
  - <a href="secure_policy.html#establishing-secure-connection" class="reference internal">Establishing secure connection</a>
- <a href="application_certificate.html" class="reference internal">Application Certificate</a>
  - <a href="application_certificate.html#creating-certificate-with-xlua" class="reference internal">Creating certificate with xlua</a>
  - <a href="application_certificate.html#creating-openssl-certificate" class="reference internal">Creating OpenSSL certificate</a>
- <a href="types.html" class="reference internal">Data Types</a>
  - <a href="types.html#built-in-types" class="reference internal">Built-in Types</a>
  - <a href="types.html#localizedtext" class="reference internal">LocalizedText</a>
  - <a href="types.html#qualifiedname" class="reference internal">QualifiedName</a>
  - <a href="types.html#nodeid" class="reference internal">NodeId</a>
    - <a href="types.html#nodeid-helpers" class="reference internal">NodeId helpers</a>
  - <a href="types.html#variant" class="reference internal">Variant</a>
    - <a href="types.html#varianttype-constants" class="reference internal">VariantType constants</a>
  - <a href="types.html#datavalue" class="reference internal">DataValue</a>
  - <a href="types.html#extensionobject" class="reference internal">ExtensionObject</a>
  - <a href="types.html#service-structures" class="reference internal">Service Structures</a>
    - <a href="types.html#activatesessionresponse" class="reference internal">ActivateSessionResponse</a>
    - <a href="types.html#addnodesresponse" class="reference internal">AddNodesResponse</a>
    - <a href="types.html#browseparameters" class="reference internal">BrowseParameters</a>
    - <a href="types.html#browseresult" class="reference internal">BrowseResult</a>
    - <a href="types.html#closesecurechannelresponse" class="reference internal">CloseSecureChannelResponse</a>
    - <a href="types.html#closesessionresponse" class="reference internal">CloseSessionResponse</a>
    - <a href="types.html#createsessionresponse" class="reference internal">CreateSessionResponse</a>
    - <a href="types.html#findserversresponse" class="reference internal">FindServersResponse</a>
    - <a href="types.html#getendpointsresponse" class="reference internal">GetEndpointsResponse</a>
    - <a href="types.html#opensecurechannelresponse" class="reference internal">OpenSecureChannelResponse</a>
    - <a href="types.html#readresponse" class="reference internal">ReadResponse</a>
    - <a href="types.html#writeresponse" class="reference internal">WriteResponse</a>
- <a href="interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>
  - <a href="interoperability_notes.html#nodeid-representation-depends-on-the-encoding" class="reference internal">NodeId representation depends on the encoding</a>
  - <a href="interoperability_notes.html#compact-binary-nodeids-are-encoding-choices" class="reference internal">Compact binary NodeIds are encoding choices</a>
  - <a href="interoperability_notes.html#namespace-indexes-are-local-to-an-address-space" class="reference internal">Namespace indexes are local to an address space</a>
  - <a href="interoperability_notes.html#nodeset2-xml-imports-require-namespace-remapping" class="reference internal">NodeSet2 XML imports require namespace remapping</a>
  - <a href="interoperability_notes.html#practical-guidance" class="reference internal">Practical guidance</a>

</div>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="about.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="About opcua-lua">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/integrations/esp32.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="index.html" class="reference internal">Examples</a>
  - <a href="learning_examples.html" class="reference internal">Learning OPC UA with Mako Server</a>
  - <a href="thirdparty_clients.html" class="reference internal">How to Connect Third-Party Clients</a>
  - <a href="#" class="current reference internal">ESP32 Microcontroller Examples</a>
    - <a href="#ready-to-run-opc-ua-server-example" class="reference internal">Ready-to-run OPC UA Server Example</a>
    - <a href="#ignition-scada" class="reference internal">Ignition SCADA</a>
      - <a href="#add-the-esp32-opcua-server-to-ignition" class="reference internal">Add the ESP32 OPCUA server to Ignition</a>
      - <a href="#ready-to-use-ignition-visualization-project" class="reference internal">Ready-to-use Ignition Visualization Project</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Examples](index.html)
- ESP32 Microcontroller Examples
- <a href="../_sources/integrations/esp32.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="esp32-microcontroller-examples" class="section">

# ESP32 Microcontroller Examples<a href="#esp32-microcontroller-examples" class="headerlink" title="Link to this heading"></a>

The benefit of using the <a href="https://realtimelogic.com/downloads/bas/ESP32/?bas=" class="reference external">Xedge32</a> examples over the <a href="https://makoserver.net/" class="reference external">Mako Server</a> examples is that these examples demonstrate how to connect an OPC UA server to real hardware. The LED and button OPCUA server example uses the <a href="https://realtimelogic.com/downloads/bas/ESP32/?bas=" class="reference external">Xedge32</a> Lua <a href="https://realtimelogic.com/ba/ESP32/source/GPIO.html" class="reference external">GPIO API</a> to connect to real hardware.

For this example, an ESP32-S3 is required as only the ready-to-run ESP32-S3 firmware includes the OPCUA stack. The <a href="https://realtimelogic.com/downloads/bas/ESP32/?bas=" class="reference external">Xedge32 installation page</a> explains how to purchase an ESP32-S3. In addition to the ESP32-S3, you will need a breadboard, LEDs, and buttons, which are typically included in electronic starter kits. For more information on wiring the diagram shown below, see the tutorial <a href="https://realtimelogic.com/articles/Your-First-Xedge32-Project" class="reference external">Your First Xedge32 Project</a>, which explains how to get an electronic starter kit and wire up the components.

In the following diagram, pulldown resistors are used, but they are often not required since most ESP32s include support for internal programmable pullup and pulldown resistors via the Lua GPIO API.

**Components and GPIOs used:**

- **GPIO4** Red LED

- **GPIO5** Green LED

- **GPIO6** Blue LED

- **GPIO7** Button

<figure class="align-center">
<img src="../_images/esp32-led-button-scheme.png" alt="Xedge32 LED and Button" />
</figure>

You may choose any GPIO available on the ESP32, but if you use other GPIO pins, make sure to change the ready-to-run OPCUA server program accordingly.

<div id="ready-to-run-opc-ua-server-example" class="section">

## Ready-to-run OPC UA Server Example<a href="#ready-to-run-opc-ua-server-example" class="headerlink" title="Link to this heading"></a>

Download the <a href="https://raw.githubusercontent.com/RealTimeLogic/OPC-UA-Examples/master/esp32/ignition/opc-led-button-server.xlua" class="reference external">Ready-to-run OPC UA ESP32 Server Example from GitHub</a>, create an <a href="https://realtimelogic.com/downloads/bas/ESP32/?bas=" class="reference external">Xedge32</a> application, and transfer the example to <a href="https://realtimelogic.com/downloads/bas/ESP32/?bas=" class="reference external">Xedge32</a>. The tutorial <a href="https://realtimelogic.com/articles/Your-First-Xedge32-Project" class="reference external">Your First Xedge32 Project</a> and the <a href="https://realtimelogic.com/products/xedge/" class="reference external">Xedge</a> documentation both explain how to create an <a href="https://realtimelogic.com/products/xedge/" class="reference external">Xedge</a> app.

In this example, the GPIO (hardware) state is accessible via the server’s OPC UA address space. Any OPC UA client can control the LEDs and read the button state. In the next sections, we will explain how to connect two OPCUA clients to the server’s address space.

</div>

<div id="ignition-scada" class="section">

## Ignition SCADA<a href="#ignition-scada" class="headerlink" title="Link to this heading"></a>

<a href="https://inductiveautomation.com/" class="reference external">Ignition</a> is a powerful SCADA (Supervisory Control and Data Acquisition) platform that enables users to design and deploy industrial automation solutions. It provides tools for building HMIs, SCADA systems, and IIoT applications, allowing seamless integration with various devices and systems.

The example project is a practical demonstration of a SCADA visualization system designed for the Ignition platform. This system effectively controls the LEDs, with the state of each LED displayed in Ignition. It also allows users to monitor the state of the button, indicating whether it is pressed or not.

To set up this project, you need to perform these steps:

1.  Add an OPCUA server to Ignition.

2.  Select the Red, Green, Blue, and Button tags and create a new polled group for these tags.

3.  Import the visualization project.

4.  Import images using Image Management.

5.  Run the project.

<div id="add-the-esp32-opcua-server-to-ignition" class="section">

### Add the ESP32 OPCUA server to Ignition<a href="#add-the-esp32-opcua-server-to-ignition" class="headerlink" title="Link to this heading"></a>

To begin, add the ESP32 OPC UA server you should now have running into Ignition. Once the ESP32 is connected in Ignition, you can use tags within the visualization to retrieve data directly from the OPC UA server.

To add the OPC UA server, follow these steps:

  

1.  Navigate to *Config* -\> *OPC Connections* in the Ignition server interface.

2.  Click on Create *New OPC Connection*….

3.  Configure the connection settings to establish communication with the OPC UA server running on the ESP32. Use the address opc.tcp://ip-address:4841. Use the IP address and not the <a href="https://realtimelogic.com/downloads/bas/ESP32/?bas=" class="reference external">Xedge32</a> <a href="https://en.wikipedia.org/wiki/Multicast_DNS" class="reference external">mDNS name</a> xedge32.local. You can find the IP address by pinging xedge32.local in a console window, e.g., ping xedge32.local.

4.  After adding an OPC UA server, ensure that the state transitions from ‘connecting’ to ‘connected’. During this connection phase, the Ignition platform retrieves all tags from the server, which may require some time.

5.  Once the ESP32 OPC UA server has been successfully added to the Ignition server, you can proceed to the next step below.

</div>

<div id="ready-to-use-ignition-visualization-project" class="section">

### Ready-to-use Ignition Visualization Project<a href="#ready-to-use-ignition-visualization-project" class="headerlink" title="Link to this heading"></a>

We have developed the Ignition visualization project, <a href="https://github.com/RealTimeLogic/OPC-UA-Examples/blob/master/esp32/ignition/ignition-led-and-button.zip" class="reference external">led_and_button_ignition.zip</a>, which can be imported into the Ignition server. To import it into Ignition, download the file, navigate to *Projects* on your Ignition server, select *Import a new project*, and import the zip file.

The following video shows all the steps plus how to enable the project.

</div>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="thirdparty_clients.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="How to Connect Third-Party Clients"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="../logging.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Logging subsystem">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/integrations/index.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="#" class="current reference internal">Examples</a>
  - <a href="learning_examples.html" class="reference internal">Learning OPC UA with Mako Server</a>
  - <a href="thirdparty_clients.html" class="reference internal">How to Connect Third-Party Clients</a>
  - <a href="esp32.html" class="reference internal">ESP32 Microcontroller Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Examples
- <a href="../_sources/integrations/index.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="examples" class="section">

# Examples<a href="#examples" class="headerlink" title="Link to this heading"></a>

We offer a variety of ready-to-run <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/OPC-UA" class="reference external">OPC UA examples on GitHub</a> for download and experimentation. Start with the README in that directory; it explains the example layout and the <a href="https://makoserver.net/" class="reference external">Mako Server</a> learning examples.

The examples are tailored for our two pre-compiled and ready-to-use Barracuda App Server products:

1.  <a href="https://makoserver.net/" class="reference external">Mako Server</a>: Suitable for high-level operating systems with pre-compiled binaries available for Windows, Linux, and many embedded Linux devices, including Raspberry Pi.

2.  <a href="https://realtimelogic.com/downloads/bas/ESP32/?bas=" class="reference external">Xedge32</a>: A web-based IDE for the ESP32 microcontroller. The ESP32 development boards are cost-effective and user-friendly. You can install the <a href="https://realtimelogic.com/downloads/bas/ESP32/?bas=" class="reference external">Xedge32</a> firmware directly onto the ESP32 from your browser.

<div class="toctree-wrapper compound">

- <a href="learning_examples.html" class="reference internal">Learning OPC UA with Mako Server</a>
  - <a href="learning_examples.html#run-the-tutorial-examples" class="reference internal">Run the tutorial examples</a>
  - <a href="learning_examples.html#run-the-pubsub-tutorial-examples" class="reference internal">Run the PubSub tutorial examples</a>
  - <a href="learning_examples.html#minimal-server" class="reference internal">1. Minimal server</a>
  - <a href="learning_examples.html#add-a-folder" class="reference internal">2. Add a folder</a>
  - <a href="learning_examples.html#add-variables" class="reference internal">3. Add variables</a>
  - <a href="learning_examples.html#write-a-variable" class="reference internal">4. Write a variable</a>
  - <a href="learning_examples.html#use-a-value-callback" class="reference internal">5. Use a value callback</a>
  - <a href="learning_examples.html#read-with-a-local-client" class="reference internal">6. Read with a local client</a>
  - <a href="learning_examples.html#write-with-a-local-client" class="reference internal">7. Write with a local client</a>
  - <a href="learning_examples.html#call-a-method" class="reference internal">8. Call a method</a>
  - <a href="learning_examples.html#define-a-structure-type" class="reference internal">9. Define a structure type</a>
  - <a href="learning_examples.html#publish-json-with-mqtt-pubsub" class="reference internal">10. Publish JSON with MQTT PubSub</a>
  - <a href="learning_examples.html#publish-binary-uadp-with-mqtt-pubsub" class="reference internal">11. Publish binary UADP with MQTT PubSub</a>
  - <a href="learning_examples.html#publish-an-opc-ua-server-node" class="reference internal">12. Publish an OPC UA server node</a>
  - <a href="learning_examples.html#more-advanced-pubsub-examples" class="reference internal">More advanced PubSub examples</a>
  - <a href="learning_examples.html#next-steps" class="reference internal">Next steps</a>
- <a href="thirdparty_clients.html" class="reference internal">How to Connect Third-Party Clients</a>
  - <a href="thirdparty_clients.html#id1" class="reference internal">UAExpert</a>
  - <a href="thirdparty_clients.html#prosys-opc-ua" class="reference internal">ProSys OPC UA</a>
  - <a href="thirdparty_clients.html#datafeed-opcua" class="reference internal">dataFEED OPCUA</a>
- <a href="esp32.html" class="reference internal">ESP32 Microcontroller Examples</a>
  - <a href="esp32.html#ready-to-run-opc-ua-server-example" class="reference internal">Ready-to-run OPC UA Server Example</a>
  - <a href="esp32.html#ignition-scada" class="reference internal">Ignition SCADA</a>
    - <a href="esp32.html#add-the-esp32-opcua-server-to-ignition" class="reference internal">Add the ESP32 OPCUA server to Ignition</a>
    - <a href="esp32.html#ready-to-use-ignition-visualization-project" class="reference internal">Ready-to-use Ignition Visualization Project</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="../pubsub/mqtt_message_format.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="UADP Message"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="learning_examples.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Learning OPC UA with Mako Server">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/integrations/learning_examples.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="index.html" class="reference internal">Examples</a>
  - <a href="#" class="current reference internal">Learning OPC UA with Mako Server</a>
    - <a href="#run-the-tutorial-examples" class="reference internal">Run the tutorial examples</a>
    - <a href="#run-the-pubsub-tutorial-examples" class="reference internal">Run the PubSub tutorial examples</a>
    - <a href="#minimal-server" class="reference internal">1. Minimal server</a>
    - <a href="#add-a-folder" class="reference internal">2. Add a folder</a>
    - <a href="#add-variables" class="reference internal">3. Add variables</a>
    - <a href="#write-a-variable" class="reference internal">4. Write a variable</a>
    - <a href="#use-a-value-callback" class="reference internal">5. Use a value callback</a>
    - <a href="#read-with-a-local-client" class="reference internal">6. Read with a local client</a>
    - <a href="#write-with-a-local-client" class="reference internal">7. Write with a local client</a>
    - <a href="#call-a-method" class="reference internal">8. Call a method</a>
    - <a href="#define-a-structure-type" class="reference internal">9. Define a structure type</a>
    - <a href="#publish-json-with-mqtt-pubsub" class="reference internal">10. Publish JSON with MQTT PubSub</a>
    - <a href="#publish-binary-uadp-with-mqtt-pubsub" class="reference internal">11. Publish binary UADP with MQTT PubSub</a>
    - <a href="#publish-an-opc-ua-server-node" class="reference internal">12. Publish an OPC UA server node</a>
    - <a href="#more-advanced-pubsub-examples" class="reference internal">More advanced PubSub examples</a>
    - <a href="#next-steps" class="reference internal">Next steps</a>
  - <a href="thirdparty_clients.html" class="reference internal">How to Connect Third-Party Clients</a>
  - <a href="esp32.html" class="reference internal">ESP32 Microcontroller Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Examples](index.html)
- Learning OPC UA with Mako Server
- <a href="../_sources/integrations/learning_examples.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="learning-opc-ua-with-mako-server" class="section">

# Learning OPC UA with Mako Server<a href="#learning-opc-ua-with-mako-server" class="headerlink" title="Link to this heading"></a>

This tutorial builds the OPC UA Lua API in small steps. Each example runs with <a href="https://makoserver.net/" class="reference external">Mako Server</a> and uses only local data, so no PLC, sensor, external OPC UA server, or hardware setup is required.

The goal is to learn the API shape:

- create a server

- add folders, variables, and methods to the address space

- read and write values

- connect a local client to the local server

<div id="run-the-tutorial-examples" class="section">

## Run the tutorial examples<a href="#run-the-tutorial-examples" class="headerlink" title="Link to this heading"></a>

The source files are in <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/OPC-UA" class="reference external">LSP-Examples/OPC-UA</a>. From that directory, run:

<div class="highlight-bash notranslate">

<div class="highlight">

    mako tutorial/run_standalone.lua

</div>

</div>

The standalone runner executes the server-side examples that do not need a long-running server process. It exits through <span class="pre">`mako.exit()`</span>. A non-zero process exit means at least one example failed.

</div>

<div id="run-the-pubsub-tutorial-examples" class="section">

## Run the PubSub tutorial examples<a href="#run-the-pubsub-tutorial-examples" class="headerlink" title="Link to this heading"></a>

The PubSub examples use the optional Lua MQTT broker module. The easiest way to test them is to use the <a href="https://makoserver.net/documentation/developer-package/" class="reference external">Mako Server mako.zip Developer Edition</a>, which includes the broker. No external MQTT broker is required for these tutorial examples.

From the <span class="pre">`LSP-Examples/OPC-UA`</span> directory, run:

<div class="highlight-bash notranslate">

<div class="highlight">

    cd tutorial
    mako run_pubsub.lua

</div>

</div>

The PubSub examples load shared tutorial code from <span class="pre">`tutorial/.lua/pubsub_common.lua`</span> using <span class="pre">`mako.createloader(io)`</span> and <span class="pre">`require()`</span>. They stop immediately and print an error message if the <span class="pre">`mqttbroker`</span> module cannot be loaded. In that case, install the Developer Edition <span class="pre">`mako.zip`</span> or copy the broker from <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/MQTT-Broker" class="reference external">MQTT-Broker</a>.

</div>

<div id="minimal-server" class="section">

## 1. Minimal server<a href="#minimal-server" class="headerlink" title="Link to this heading"></a>

Start with the smallest useful server. The server has one <span class="pre">`opc.tcp`</span> endpoint, allows the <span class="pre">`None`</span> security policy, starts listening, and then shuts down.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")
    local server = ua.newServer()
    server:initialize()

    trace("Server initialized with one opc.tcp endpoint.")

    server:run()
    server:shutdown()

</div>

</div>

</div>

<div id="add-a-folder" class="section">

## 2. Add a folder<a href="#add-a-folder" class="headerlink" title="Link to this heading"></a>

OPC UA data is organized in the address space. A common first step is to add a folder under the standard <span class="pre">`Objects`</span> folder.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")


    local server = ua.newServer()
    server:initialize()

    local editor = server.model:edit()
    local objects = editor:objectsFolder()
    local tutorial = objects:addFolder("Tutorial")
    tutorial.Attrs.Description = {
      Text = "Folder used by the Mako Server learning examples"
    }
    editor:save()

    trace("Added Tutorial folder under Objects.")

    server:run()
    server:shutdown()

</div>

</div>

</div>

<div id="add-variables" class="section">

## 3. Add variables<a href="#add-variables" class="headerlink" title="Link to this heading"></a>

Variables expose values. This example adds two <span class="pre">`Double`</span> variables with stable NodeIds so later examples can read and write them directly.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local temperatureId = "ns=1;i=3001"
    local setpointId = "ns=1;i=3002"

    local server = ua.newServer()
    server:initialize()

    local editor = server.model:edit()
    local tutorial = editor:objectsFolder():addFolder("Tutorial")

    tutorial:addVariable("Temperature", {
      Type = ua.VariantType.Double,
      Value = 21.5
    }, nil, temperatureId)

    tutorial:addVariable("Setpoint", {
      Type = ua.VariantType.Double,
      Value = 24.0
    }, nil, setpointId)

    editor:save()

    local resp = server:read({
      {
        NodeId = temperatureId,
        AttributeId = ua.AttributeId.Value
      }
    })
    assert(resp.Results[1].Value == 21.5)

    trace("Added Temperature and Setpoint variables.")

    server:run()
    server:shutdown()

</div>

</div>

</div>

<div id="write-a-variable" class="section">

## 4. Write a variable<a href="#write-a-variable" class="headerlink" title="Link to this heading"></a>

Server-side Lua can write a variable through the same service shape used by remote clients. This is useful when application logic updates the address space.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local setpointId = "ns=1;i=4001"

    local server = ua.newServer()
    server:initialize()

    local editor = server.model:edit()
    local tutorial = editor:objectsFolder():addFolder("Tutorial")
    tutorial:addVariable("Setpoint", {
      Type = ua.VariantType.Double,
      Value = 24.0
    }, nil, setpointId)
    editor:save()

    local writeResp = server:write({
      NodesToWrite = {
        {
          NodeId = setpointId,
          AttributeId = ua.AttributeId.Value,
          Value = {
            Type = ua.VariantType.Double,
            Value = 26.5,
            StatusCode = ua.StatusCode.Good
          }
        }
      }
    })
    assert(writeResp.Results[1] == ua.StatusCode.Good)

    local readResp = server:read({
      {
        NodeId = setpointId,
        AttributeId = ua.AttributeId.Value
      }
    })
    assert(readResp.Results[1].Value == 26.5)

    trace("Wrote Setpoint from server-side Lua.")

    server:run()
    server:shutdown()

</div>

</div>

</div>

<div id="use-a-value-callback" class="section">

## 5. Use a value callback<a href="#use-a-value-callback" class="headerlink" title="Link to this heading"></a>

A value callback lets a variable read from and write to application state instead of storing only a fixed value in the model.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local counterId = "ns=1;i=5001"
    local counter = {
      Type = ua.VariantType.UInt32,
      Value = 0,
      StatusCode = ua.StatusCode.Good
    }

    local server = ua.newServer()
    server:initialize()

    local editor = server.model:edit()
    local tutorial = editor:objectsFolder():addFolder("Tutorial")
    tutorial:addVariable("Counter", counter, nil, counterId)
    editor:save()

    server:setValueCallback(counterId, function(nodeId, newValue)
      if newValue then
        counter = newValue
        return
      end

      counter.Value = counter.Value + 1
      return counter
    end)

    local first = server:read({
      {
        NodeId = counterId,
        AttributeId = ua.AttributeId.Value
      }
    })
    local firstValue = first.Results[1].Value

    local second = server:read({
      {
        NodeId = counterId,
        AttributeId = ua.AttributeId.Value
      }
    })
    assert(firstValue == 1)
    assert(second.Results[1].Value == 2)

    trace("Counter value is produced by a Lua callback.")

    server:run()
    server:shutdown()

</div>

</div>

</div>

<div id="read-with-a-local-client" class="section">

## 6. Read with a local client<a href="#read-with-a-local-client" class="headerlink" title="Link to this heading"></a>

The client examples use a separate local learning server. Start it in one terminal:

<div class="highlight-bash notranslate">

<div class="highlight">

    mako -l::tutorial/learning_server

</div>

</div>

Then run the client scripts from another terminal. The first client example opens a secure channel, creates a session, activates it, and reads one value. Stop the learning server with <span class="pre">`Ctrl+C`</span> when you are done.

<div id="id3" class="literal-block-wrapper docutils container">

<div class="code-block-caption">

<span class="caption-text">Local learning server used by examples 6 through 8</span><a href="#id3" class="headerlink" title="Link to this code"></a>

</div>

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local endpointUrl = "opc.tcp://localhost:4850"
    local temperatureId = "ns=1;i=6001"
    local setpointId = "ns=1;i=7001"
    local controllerId = "ns=1;i=8001"
    local methodId = "ns=1;i=8002"

    local config = {
      applicationName = "Mako learning server",
      applicationUri = "urn:opcua-lua:mako-learning-server",
      productUri = "urn:opcua-lua:mako-learning-server",
      bufSize = 65536,

      endpoints = {
        {
          listenPort = 4850,
          listenAddress = "localhost",
          endpointUrl = endpointUrl
        }
      },

      securePolicies = {
        {
          securityPolicyUri = ua.SecurityPolicy.None
        }
      },

      userIdentityTokens = {
        {
          policyId = "anonymous",
          tokenType = 0
        }
      }
    }

    local server = ua.newServer(config)
    server:initialize()

    local editor = server.model:edit()
    local tutorial = editor:objectsFolder():addFolder("Tutorial")

    tutorial:addVariable("Temperature", {
      Type = ua.VariantType.Double,
      Value = 21.5
    }, nil, temperatureId)

    tutorial:addVariable("Setpoint", {
      Type = ua.VariantType.Double,
      Value = 24.0
    }, nil, setpointId)

    local controller = tutorial:addObject("Controller", nil, controllerId)
    local function scaleValue(objectId, calledMethodId, inputArguments)
      local value = inputArguments[1].Value
      local factor = inputArguments[2].Value

      return {
        {
          Type = ua.VariantType.Double,
          Value = value * factor
        }
      }
    end

    controller:addMethod(
      "ScaleValue",
      scaleValue,
      {
        {Name = "Value", DataType = ua.DataTypeId.Double},
        {Name = "Factor", DataType = ua.DataTypeId.Double}
      },
      {
        {Name = "Scaled", DataType = ua.DataTypeId.Double}
      },
      methodId
    )

    editor:save()

    trace("Learning server ready at " .. endpointUrl)
    server:run()

    function onunload()
      trace("Stopping learning server.")
      server:shutdown()
    end

</div>

</div>

</div>

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local endpointUrl = "opc.tcp://localhost:4850"
    local temperatureId = "ns=1;i=6001"

    local config = {
      applicationName = "Mako learning client",
      applicationUri = "urn:opcua-lua:mako-learning-client",
      productUri = "urn:opcua-lua:mako-learning-client",
      cosocketMode = false,
      socketTimeout = 10000,
      securePolicies = {
        {
          securityPolicyUri = ua.SecurityPolicy.None
        }
      }
    }

    local client = ua.newClient(config)
    local err = client:connect(endpointUrl)
    assert(err == nil, tostring(err))

    local resp
    resp, err = client:openSecureChannel(
      120000,
      ua.SecurityPolicy.None,
      ua.MessageSecurityMode.None
    )
    assert(err == nil, tostring(err))

    resp, err = client:createSession("mako_learning_read", 120000)
    assert(err == nil, tostring(err))

    resp, err = client:activateSession()
    assert(err == nil, tostring(err))

    resp, err = client:read({
      NodesToRead = {
        {
          NodeId = temperatureId,
          AttributeId = ua.AttributeId.Value
        }
      }
    })
    assert(err == nil, tostring(err))
    assert(resp.Results[1].Value == 21.5)

    trace("Client read Temperature from the learning server.")

    client:closeSession()
    client:disconnect()

</div>

</div>

</div>

<div id="write-with-a-local-client" class="section">

## 7. Write with a local client<a href="#write-with-a-local-client" class="headerlink" title="Link to this heading"></a>

Writing from a client uses <span class="pre">`client:write()`</span> with one or more nodes to update. The example verifies the result from the server side after the write completes.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local endpointUrl = "opc.tcp://localhost:4850"
    local setpointId = "ns=1;i=7001"

    local config = {
      applicationName = "Mako learning client",
      applicationUri = "urn:opcua-lua:mako-learning-client",
      productUri = "urn:opcua-lua:mako-learning-client",
      cosocketMode = false,
      socketTimeout = 10000,
      securePolicies = {
        {
          securityPolicyUri = ua.SecurityPolicy.None
        }
      }
    }

    local client = ua.newClient(config)
    local err = client:connect(endpointUrl)
    assert(err == nil, tostring(err))

    local resp
    resp, err = client:openSecureChannel(
      120000,
      ua.SecurityPolicy.None,
      ua.MessageSecurityMode.None
    )
    assert(err == nil, tostring(err))

    resp, err = client:createSession("mako_learning_write", 120000)
    assert(err == nil, tostring(err))

    resp, err = client:activateSession()
    assert(err == nil, tostring(err))

    resp, err = client:write({
      NodesToWrite = {
        {
          NodeId = setpointId,
          AttributeId = ua.AttributeId.Value,
          Value = {
            Type = ua.VariantType.Double,
            Value = 25.25,
            StatusCode = ua.StatusCode.Good
          }
        }
      }
    })
    assert(err == nil, tostring(err))
    assert(resp.Results[1] == ua.StatusCode.Good)

    resp, err = client:read({
      NodesToRead = {
        {
          NodeId = setpointId,
          AttributeId = ua.AttributeId.Value
        }
      }
    })
    assert(err == nil, tostring(err))
    assert(resp.Results[1].Value == 25.25)

    trace("Client wrote Setpoint on the learning server.")

    client:closeSession()
    client:disconnect()

</div>

</div>

</div>

<div id="call-a-method" class="section">

## 8. Call a method<a href="#call-a-method" class="headerlink" title="Link to this heading"></a>

Methods are callable operations attached to objects. This example exposes <span class="pre">`ScaleValue`</span> on a local <span class="pre">`Controller`</span> object and calls it from the client.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local endpointUrl = "opc.tcp://localhost:4850"
    local controllerId = "ns=1;i=8001"
    local methodId = "ns=1;i=8002"

    local config = {
      applicationName = "Mako learning client",
      applicationUri = "urn:opcua-lua:mako-learning-client",
      productUri = "urn:opcua-lua:mako-learning-client",
      cosocketMode = false,
      socketTimeout = 10000,
      securePolicies = {
        {
          securityPolicyUri = ua.SecurityPolicy.None
        }
      }
    }

    local client = ua.newClient(config)
    local err = client:connect(endpointUrl)
    assert(err == nil, tostring(err))

    local resp
    resp, err = client:openSecureChannel(
      120000,
      ua.SecurityPolicy.None,
      ua.MessageSecurityMode.None
    )
    assert(err == nil, tostring(err))

    resp, err = client:createSession("mako_learning_call", 120000)
    assert(err == nil, tostring(err))

    resp, err = client:activateSession()
    assert(err == nil, tostring(err))

    resp, err = client:call(controllerId, methodId, {
      {Type = ua.VariantType.Double, Value = 10.0},
      {Type = ua.VariantType.Double, Value = 2.5}
    })
    assert(err == nil, tostring(err))
    assert(resp.Results[1].StatusCode == ua.StatusCode.Good)
    assert(resp.Results[1].OutputArguments[1].Value == 25.0)

    trace("Client called ScaleValue on the learning server.")

    client:closeSession()
    client:disconnect()

</div>

</div>

</div>

<div id="define-a-structure-type" class="section">

## 9. Define a structure type<a href="#define-a-structure-type" class="headerlink" title="Link to this heading"></a>

OPC UA models can define custom data types. This example creates a small <span class="pre">`MeasurementType`</span> structure and a variable type that uses it.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local server = ua.newServer()
    server:initialize()

    local editor = server.model:edit()

    local measurementType = editor:addStructure("MeasurementType")
    measurementType:addField("Temperature", ua.DataTypeId.Double, ua.ValueRank.Scalar)
    measurementType:addField("Humidity", ua.DataTypeId.Double, ua.ValueRank.Scalar)

    local measurementVariableType = editor:addVariableType(
      "MeasurementVariableType",
      nil,
      measurementType
    )

    local tutorial = editor:objectsFolder():addFolder("Tutorial")
    local measurement = tutorial:addVariable(
      "Measurement",
      nil,
      measurementVariableType,
      "ns=1;i=9001"
    )
    measurement.Attrs.Description = {
      Text = "Structured measurement value"
    }

    editor:save()

    local fields = measurementType:getFields()
    assert(#fields == 2)
    assert(fields[1].Name == "Temperature")
    assert(fields[2].Name == "Humidity")

    trace("Added a custom structure and a variable type that uses it.")

    server:run()
    server:shutdown()

</div>

</div>

<div class="admonition warning">

Warning

The following PubSub examples require an <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/MQTT-Broker" class="reference external">MQTT broker</a>. The easiest test setup is the <a href="https://makoserver.net/documentation/developer-package/" class="reference external">Mako Server mako.zip Developer Edition</a>, which includes the broker module used by these examples.

</div>

</div>

<div id="publish-json-with-mqtt-pubsub" class="section">

## 10. Publish JSON with MQTT PubSub<a href="#publish-json-with-mqtt-pubsub" class="headerlink" title="Link to this heading"></a>

This example creates a local MQTT broker, connects an OPC UA MQTT publisher and subscriber through the broker’s in-process client API, publishes one JSON dataset message, and verifies that the subscriber decodes the OPC UA PubSub payload.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local io = ba.openio("home")
    mako.createloader(io)
    local common = require("pubsub_common")

    local brokerPort = 18891
    local broker = common.createBroker(brokerPort)
    local topic = "opcua/tutorial/json"
    local transportProfileUri = ua.TranportProfileUri.MqttJson

    local config = {
      bufSize = 8192
    }

    local subscriber = ua.newMqttClient(config)
    local received
    common.connectLocal(subscriber, broker, transportProfileUri)

    subscriber:subscribe(topic, function(message, err)
      if err then
        common.fail("Failed to decode MQTT JSON PubSub message: " .. tostring(err))
      end
      received = message
    end)

    ba.sleep(300)

    local publisher = ua.newMqttClient(config)
    local datasetId = publisher:createDataset({
      { name = "Temperature" }
    })

    common.connectLocal(publisher, broker, transportProfileUri)
    publisher:setValue(datasetId, "Temperature", {
      Type = ua.VariantType.Double,
      Value = 21.5
    })
    publisher:publish(topic, "tutorial-json")

    common.waitFor("JSON PubSub message", function() return received ~= nil end)

    local value = received.Messages[1].Payload.Temperature
    assert(value.Type == ua.VariantType.Double)
    assert(value.Value == 21.5)

    publisher:close()
    subscriber:close()
    broker:shutdown()

    trace("JSON PubSub message received and decoded.")

</div>

</div>

</div>

<div id="publish-binary-uadp-with-mqtt-pubsub" class="section">

## 11. Publish binary UADP with MQTT PubSub<a href="#publish-binary-uadp-with-mqtt-pubsub" class="headerlink" title="Link to this heading"></a>

The binary example uses the same broker setup but switches the transport profile to MQTT binary/UADP. It publishes two fields and verifies the decoded field indexes and values.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local io = ba.openio("home")
    mako.createloader(io)
    local common = require("pubsub_common")

    local brokerPort = 18892
    local broker = common.createBroker(brokerPort)
    local topic = "opcua/tutorial/uadp"
    local transportProfileUri = ua.TranportProfileUri.MqttBinary

    local config = {
      bufSize = 8192
    }

    local subscriber = ua.newMqttClient(config)
    local received
    common.connectLocal(subscriber, broker, transportProfileUri)

    subscriber:subscribe(topic, function(message, err)
      if err then
        common.fail("Failed to decode MQTT binary PubSub message: " .. tostring(err))
      end
      received = message
    end)

    ba.sleep(300)

    local publisher = ua.newMqttClient(config)
    local datasetId = publisher:createDataset({
      { name = "Speed" },
      { name = "State" }
    })

    common.connectLocal(publisher, broker, transportProfileUri)
    publisher:setValue(datasetId, "Speed", {
      Type = ua.VariantType.UInt32,
      Value = 1200
    })
    publisher:setValue(datasetId, "State", {
      Type = ua.VariantType.Boolean,
      Value = true
    })
    publisher:publish(topic, "tutorial-binary")

    common.waitFor("binary PubSub message", function() return received ~= nil end)

    local fields = received.Messages[1].Fields
    assert(fields[1].Index == 1)
    assert(fields[1].Value.Type == ua.VariantType.UInt32)
    assert(fields[1].Value.Value == 1200)
    assert(fields[2].Index == 2)
    assert(fields[2].Value.Type == ua.VariantType.Boolean)
    assert(fields[2].Value.Value == true)

    publisher:close()
    subscriber:close()
    broker:shutdown()

    trace("Binary PubSub message received and decoded.")

</div>

</div>

</div>

<div id="publish-an-opc-ua-server-node" class="section">

## 12. Publish an OPC UA server node<a href="#publish-an-opc-ua-server-node" class="headerlink" title="Link to this heading"></a>

The final PubSub example connects a publisher to an OPC UA server instance. A dataset field is bound to a server node, the server writes a new value, and the publisher sends the updated node value through MQTT PubSub.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local io = ba.openio("home")
    mako.createloader(io)
    local common = require("pubsub_common")

    local brokerPort = 18893
    local broker = common.createBroker(brokerPort)
    local topic = "opcua/tutorial/server-node"
    local transportProfileUri = ua.TranportProfileUri.MqttJson
    local variableId = "ns=1;s=TutorialLevel"

    local config = {
      bufSize = 8192
    }

    local server = ua.newServer({
      endpoints = {
        {
          endpointUrl = "opc.tcp://localhost:4846"
        }
      },

      securePolicies = {
        {
          securityPolicyUri = ua.SecurityPolicy.None
        }
      }
    })

    server:initialize()

    local editor = server.model:edit()
    local tutorial = editor:objectsFolder():addFolder("TutorialPubSub")
    tutorial:addVariable("Level", {
      Type = ua.VariantType.UInt32,
      Value = 1
    }, nil, variableId)
    editor:save()

    server:run()

    local subscriber = ua.newMqttClient(config)
    local received
    common.connectLocal(subscriber, broker, transportProfileUri)

    subscriber:subscribe(topic, function(message, err)
      if err then
        common.fail("Failed to decode MQTT server-node message: " .. tostring(err))
      end
      received = message
    end)

    ba.sleep(300)

    local publisher = ua.newMqttClient(config, server)
    local datasetId = publisher:createDataset({
      {
        nodeId = variableId,
        name = "Level"
      }
    })

    common.connectLocal(publisher, broker, transportProfileUri)

    server:write({
      NodesToWrite = {
        {
          NodeId = variableId,
          AttributeId = ua.AttributeId.Value,
          Value = {
            Type = ua.VariantType.UInt32,
            Value = 42
          }
        }
      }
    })

    publisher:publish(topic, "tutorial-server-node")

    common.waitFor("server-node PubSub message", function() return received ~= nil end)

    local value = received.Messages[1].Payload.Level
    assert(value.Type == ua.VariantType.UInt32)
    assert(value.Value == 42)

    publisher:close()
    subscriber:close()
    server:shutdown()
    broker:shutdown()

    trace("Server node value published through MQTT PubSub.")

</div>

</div>

</div>

<div id="more-advanced-pubsub-examples" class="section">

## More advanced PubSub examples<a href="#more-advanced-pubsub-examples" class="headerlink" title="Link to this heading"></a>

After the tutorial examples, see <a href="../pubsub/mqtt_examples.html" class="reference internal"><span class="doc">MQTT PubSub Examples</span></a> for more compact PubSub API examples. They cover publishing server node changes, manual publishing without an OPC UA server, and subscribing to JSON and binary/UADP messages.

</div>

<div id="next-steps" class="section">

## Next steps<a href="#next-steps" class="headerlink" title="Link to this heading"></a>

After these examples, the reference pages are easier to read:

- <a href="../model/index.html" class="reference internal"><span class="doc">Address Space API</span></a> for address-space editing

- <a href="../client/index.html" class="reference internal"><span class="doc">Client</span></a> for client sessions and service calls

- <a href="../server/index.html" class="reference internal"><span class="doc">Server</span></a> for server configuration and model setup

- <a href="../pubsub/index.html" class="reference internal"><span class="doc">Publish Subscribe API</span></a> for PubSub concepts and MQTT API details

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="index.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Examples"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="thirdparty_clients.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="How to Connect Third-Party Clients">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/integrations/thirdparty_clients.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="index.html" class="reference internal">Examples</a>
  - <a href="learning_examples.html" class="reference internal">Learning OPC UA with Mako Server</a>
  - <a href="#" class="current reference internal">How to Connect Third-Party Clients</a>
    - <a href="#id1" class="reference internal">UAExpert</a>
    - <a href="#prosys-opc-ua" class="reference internal">ProSys OPC UA</a>
    - <a href="#datafeed-opcua" class="reference internal">dataFEED OPCUA</a>
  - <a href="esp32.html" class="reference internal">ESP32 Microcontroller Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Examples](index.html)
- How to Connect Third-Party Clients
- <a href="../_sources/integrations/thirdparty_clients.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="how-to-connect-third-party-clients" class="section">


# How to Connect Third-Party Clients<a href="#how-to-connect-third-party-clients" class="headerlink" title="Link to this heading"></a>

In this tutorial, we will show how to use third-party OPC UA clients when connecting to our OPC UA server.

We use the OPC URL opc.tcp://localhost:4841 in the following tutorial. This URL is the one exported by the <a href="https://github.com/RealTimeLogic/OPC-UA-Examples/tree/master/servers" class="reference external">Demo Server</a>.

You may connect to the OPC UA Demo Server with any OPC UA client. We are using the following clients in this tutorial:

> <div>
>
> - <a href="https://www.unified-automation.com/downloads/opc-ua-clients.html" class="reference external">UAExpert</a>
>
> - <a href="https://www.prosysopc.com/products/opc-ua-browser" class="reference external">ProSys OPC UA Client</a>
>
> - <a href="https://data-intelligence.softing.com/us/products/datafeed-opc-sdks/opc-ua-demo-client/" class="reference external">dataFeed OPC UA client</a>
>
> </div>

<div id="id1" class="section">

## UAExpert<a href="#id1" class="headerlink" title="Link to this heading"></a>

After starting UAExpert, click on the plus icon button.


Double click <span class="guilabel">Double click to Add Server</span> in the popup window.


Enter endpoint URL <span class="pre">`opc.tcp://localhost:4841`</span> and click <span class="guilabel">OK</span>.


Expand the subtree under endpoint URL, select <span class="guilabel">None - None</span>, and click <span class="guilabel">OK</span>.


The left pane shows the OPC UA server name. Connect to the OPC UA server by clicking on the connect icon as shown in the following screenshot.


The address space is shown in the left pane after connecting. The right pane and the upper pane shows attributes of selected node.


</div>

<div id="prosys-opc-ua" class="section">

## ProSys OPC UA<a href="#prosys-opc-ua" class="headerlink" title="Link to this heading"></a>

Start the ProSys OPCUA client application. Enter Endpoint URL and click on the arrow icon.


The following screenshot shows the popup that appears after clicking the arrow icon. Select security mode <span class="guilabel">None</span> and security policy <span class="guilabel">None</span>. Click <span class="guilabel">OK</span>.


The address space is shown in the left pane after connecting. The middle pane shows attributes of selected node and the right pane shows references of selected node.


</div>

<div id="datafeed-opcua" class="section">

## dataFEED OPCUA<a href="#datafeed-opcua" class="headerlink" title="Link to this heading"></a>

Start the dataFEED OPC UA client


Double click to add session. Add the following Endpoint URL in the popup window: <span class="pre">`opc.tcp://localhost:4841`</span> and click <span class="guilabel">OK</span>.


After connecting, the middle pane shows the Address Space tree and the left pane shows the attributes of the selected node.


</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="learning_examples.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Learning OPC UA with Mako Server"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="esp32.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="ESP32 Microcontroller Examples">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/interoperability_notes.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="about.html" class="reference internal">About opcua-lua</a>
- <a href="opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="model/index.html" class="reference internal">Address Space API</a>
- <a href="client/index.html" class="reference internal">Client</a>
- <a href="server/index.html" class="reference internal">Server</a>
- <a href="pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="integrations/index.html" class="reference internal">Examples</a>
- <a href="logging.html" class="reference internal">Logging subsystem</a>
- <a href="secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="types.html" class="reference internal">Data Types</a>
- <a href="#" class="current reference internal">OPC UA interoperability notes</a>
  - <a href="#nodeid-representation-depends-on-the-encoding" class="reference internal">NodeId representation depends on the encoding</a>
  - <a href="#compact-binary-nodeids-are-encoding-choices" class="reference internal">Compact binary NodeIds are encoding choices</a>
  - <a href="#namespace-indexes-are-local-to-an-address-space" class="reference internal">Namespace indexes are local to an address space</a>
  - <a href="#nodeset2-xml-imports-require-namespace-remapping" class="reference internal">NodeSet2 XML imports require namespace remapping</a>
  - <a href="#practical-guidance" class="reference internal">Practical guidance</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="index.html" class="icon icon-home" aria-label="Home"></a>
- OPC UA interoperability notes
- <a href="_sources/interoperability_notes.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="opc-ua-interoperability-notes" class="section">

# OPC UA interoperability notes<a href="#opc-ua-interoperability-notes" class="headerlink" title="Link to this heading"></a>

OPC UA defines a common information model and service set, but the same concept can appear in different wire formats, files, and server address spaces. This page explains the parts that most often cause implementation mistakes and shows how to handle them in a predictable way.

<div id="nodeid-representation-depends-on-the-encoding" class="section">

## NodeId representation depends on the encoding<a href="#nodeid-representation-depends-on-the-encoding" class="headerlink" title="Link to this heading"></a>

A NodeId identifies a node within a namespace. The logical value is the same concept across OPC UA, but the encoded representation is not identical in every format.

For example, OPC UA JSON encoding and UADP/UA Binary encoding use different NodeId identifier type values:

| Identifier type                       | JSON value | UADP/UA Binary value |
|---------------------------------------|------------|----------------------|
| <span class="pre">`TwoByte`</span>    | not used   | 0                    |
| <span class="pre">`FourByte`</span>   | not used   | 1                    |
| <span class="pre">`Numeric`</span>    | 0          | 2                    |
| <span class="pre">`String`</span>     | 1          | 3                    |
| <span class="pre">`Guid`</span>       | 2          | 4                    |
| <span class="pre">`ByteString`</span> | 3          | 5                    |

The two lists look similar, but they are not the same enum. A JSON NodeId type must not be decoded with the UADP or UA Binary NodeId enum, and UADP or UA Binary values must not be decoded with the JSON enum.

There is an apparent numeric relationship for some values, such as <span class="pre">`UadpType`</span>` `<span class="pre">`=`</span>` `<span class="pre">`JsonType`</span>` `<span class="pre">`+`</span>` `<span class="pre">`2`</span> for Numeric, String, Guid, and ByteString. Treat this only as an observation, not as an implementation rule. It does not cover the compact TwoByte and FourByte binary forms and it makes the decoder depend on a shortcut instead of the active encoding.

</div>

<div id="compact-binary-nodeids-are-encoding-choices" class="section">

## Compact binary NodeIds are encoding choices<a href="#compact-binary-nodeids-are-encoding-choices" class="headerlink" title="Link to this heading"></a>

The binary encodings can represent some numeric NodeIds with compact forms:

| Binary form                         | When it can be used                                                                                                                                                                                  |
|-------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <span class="pre">`TwoByte`</span>  | Namespace <span class="pre">`0`</span> numeric identifiers in the range <span class="pre">`0`</span> to <span class="pre">`255`</span>.                                                              |
| <span class="pre">`FourByte`</span> | Namespace indexes in the range <span class="pre">`0`</span> to <span class="pre">`255`</span> and numeric identifiers in the range <span class="pre">`0`</span> to <span class="pre">`65535`</span>. |
| <span class="pre">`Numeric`</span>  | Larger namespace indexes and numeric identifiers.                                                                                                                                                    |

Do not infer the original binary NodeId form from the numeric identifier alone. The same logical NodeId can often be encoded in more than one valid binary form. A decoder should preserve the logical NodeId value. An encoder may choose the smallest valid binary form unless a specific format is required by the caller or by an interoperability test.

Recommended implementation pattern:

- Decode each transport format with its own format-specific parser.

- Convert decoded values into one internal NodeId representation.

- Encode from that internal representation using the target wire format.

- Keep JSON, UADP, and UA Binary enum handling separate.

</div>

<div id="namespace-indexes-are-local-to-an-address-space" class="section">

## Namespace indexes are local to an address space<a href="#namespace-indexes-are-local-to-an-address-space" class="headerlink" title="Link to this heading"></a>

A NodeId contains a namespace index. That index is not a global identifier. It is an index into the server’s namespace table, so the same companion specification can have different indexes on different servers.

For example, two servers may expose the same type of machine and the same companion specification, but load NodeSet2 XML files in a different order. The address-space hierarchy may be equivalent while the namespace indexes in NodeIds are different.

Applications should avoid treating namespace indexes as stable across servers. Use one of these strategies instead:

- Resolve the namespace URI from the server namespace table before comparing or storing NodeIds outside the current server connection.

- Store external references as ExpandedNodeIds or as <span class="pre">`namespaceUri`</span> plus identifier.

- Use browse paths or <span class="pre">`TranslateBrowsePathsToNodeIds`</span> when a stable model path is more appropriate than a hard-coded NodeId.

- Load companion specifications in a deterministic order when you control the server configuration.

</div>

<div id="nodeset2-xml-imports-require-namespace-remapping" class="section">

## NodeSet2 XML imports require namespace remapping<a href="#nodeset2-xml-imports-require-namespace-remapping" class="headerlink" title="Link to this heading"></a>

NodeSet2 XML files contain a namespace table and NodeIds that refer to entries in that table. The namespace indexes inside a file are local to that file. When the file is imported into a server model, those indexes must be mapped to the indexes assigned by the target address space.

The default OPC UA namespace has index <span class="pre">`0`</span>. Servers also commonly reserve a local application namespace, often index <span class="pre">`1`</span>. Imported companion specifications are then assigned indexes according to the model loader’s namespace table.

Correct import logic must therefore:

- Read the namespace URIs declared by the XML file.

- Create or find the corresponding namespaces in the target model.

- Rewrite NodeIds, reference targets, type definitions, data type references, and aliases from XML-local indexes to target-model indexes.

- Preserve the namespace URI as the stable identity of the namespace.

This remapping is expected OPC UA behavior, not a data corruption issue. A NodeId string copied directly from a NodeSet2 file may not be valid in the same form after import unless the namespace index happens to be the same.

</div>

<div id="practical-guidance" class="section">

## Practical guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

For robust OPC UA applications:

- Treat NodeIds as structured values, not as plain strings.

- Treat namespace URI as the stable namespace identity.

- Use namespace indexes only after resolving them in the current server or model.

- Keep transport-specific encoding code separate from model-level NodeId logic.

- Prefer browse paths or well-known type definitions when connecting to servers that may load companion specifications in different orders.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="types.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Data Types"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/logging.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="about.html" class="reference internal">About opcua-lua</a>
- <a href="opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="model/index.html" class="reference internal">Address Space API</a>
- <a href="client/index.html" class="reference internal">Client</a>
- <a href="server/index.html" class="reference internal">Server</a>
- <a href="pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="integrations/index.html" class="reference internal">Examples</a>
- <a href="#" class="current reference internal">Logging subsystem</a>
- <a href="secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="types.html" class="reference internal">Data Types</a>
- <a href="interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="index.html" class="icon icon-home" aria-label="Home"></a>
- Logging subsystem
- <a href="_sources/logging.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="logging-subsystem" class="section">

# Logging subsystem<a href="#logging-subsystem" class="headerlink" title="Link to this heading"></a>

There are three types of message tracing that may be enabled in the OPC-UA server:

> <div>
>
> - Debugging messages
>
> - Information messages
>
> - Error messages
>
> </div>

All information is by default sent to the <a href="../doc/?url=lua.html#_G_trace" class="reference external">trace function</a>. You may override the message tracing as follows:

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")
    -- Debug messages
    ua.log.dbg = function(msg)
      print(sock.gettime(), "[DBG] ", msg)
    end

    -- Information messages
    ua.log.inf = function(msg)
      print(sock.gettime(), "[INF]", msg)
    end

    -- Errors are highlighted with red color on POSIX terminals
    ua.log.err = function(msg)
      print(sock.gettime(), "\27[31m[ERR]\27[39m", msg)
    end

</div>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="integrations/esp32.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="ESP32 Microcontroller Examples"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="secure_policy.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Secure Policy">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/model/browser.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="index.html" class="reference internal">Address Space API</a>
  - <a href="model.html" class="reference internal">Model methods</a>
  - <a href="#" class="current reference internal">Browser</a>
  - <a href="editor.html" class="reference internal">Editor</a>
  - <a href="import_export.html" class="reference internal">Import/Export</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Address Space API](index.html)
- Browser
- <a href="../_sources/model/browser.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="browser" class="section">

# Browser<a href="#browser" class="headerlink" title="Link to this heading"></a>

Browser is returned by <span class="pre">`model:browse()`</span>. It is used to browse the address space.

<span class="sig-name descname"><span class="pre">browser:children()</span></span>  
Get the children of the parent node.

Returns<span class="colon">:</span>  
Array of browser objects.

<!-- -->

<span class="sig-name descname"><span class="pre">browser:path(pathElements)</span></span>  
Resolve a path in the address space starting from the root node.

PathElements<span class="colon">:</span>  
Path elements. You can mix strings, <a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a> values, and table elements in the same path.

Strings and <a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a> values follow hierarchical references. Table elements can select a specific reference type.

Table path elements use these fields:

| Field                                      | Type                                                                                                                          | Description                                                                                                   |
|--------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------|
| <span class="pre">`TargetName`</span>      | string or <a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a> | Required browse name of the target node.                                                                      |
| <span class="pre">`ReferenceTypeId`</span> | <a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a>, optional               | Reference type to follow. Defaults to hierarchical references.                                                |
| <span class="pre">`IsInverse`</span>       | boolean, optional                                                                                                             | Follow inverse references when <span class="pre">`true`</span>. Defaults to <span class="pre">`false`</span>. |
| <span class="pre">`IncludeSubtypes`</span> | boolean, optional                                                                                                             | Include subtypes of <span class="pre">`ReferenceTypeId`</span>. Defaults to <span class="pre">`true`</span>.  |

Returns<span class="colon">:</span>  
Browser object.

Raises<span class="colon">:</span>  
if the path is not found, or other error occurs.

**Examples:**

Get one child node by browse name:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Resolve path to the objects folder node with id i=85
    2local objectsFolderPath = model:browse():path("Objects")

</div>

</div>

Follow hierarchical references:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Resolve path to the server object node
    2local serverObject = model:browse():path({"Objects", "Server"})

</div>

</div>

Mix browse names and table parameters:

<div class="highlight-lua notranslate">

<div class="highlight">

     1-- Resolve path to server object type definition node
     2-- it is possible to mix string and table in the path:
     3--   * for strings it will browsed hierarhical references
     4--   * for tables it will browsed references by the ReferenceTypeId
     5local serverObjectTypeDefinition = model:browse():path({
     6  "Objects", -- #1
     7  "Server",  -- #2
     8  {          -- #3
     9    TargetName = "ServerType", -- This is either a strinf or a QualifiedName
    10    ReferenceTypeId = ua.ReferenceType.HasTypeDefinition,
    11    IncludeSubtypes = false,
    12    IsInverse = false,
    13  }
    14})
    15
    16print(serverObjectTypeDefinition.Attrs.BrowseName.Name) -- "ServerType"
    17print(serverObjectTypeDefinition.Attrs.NodeId) -- "i=2253"

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_model_browse.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">browser:getNode(nodeId)</span></span>  
Get the browser object by identifier. Result object will have methods according to the node class.

NodeId<span class="colon">:</span>  
Node identifier.

Returns<span class="colon">:</span>  
Browser object.

<!-- -->

<span class="sig-name descname"><span class="pre">browser:objectsFolder()</span></span>  
Returns a node under which all the objects are placed.

Returns<span class="colon">:</span>  
Browser object.

**Example:**

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Get the objects folder node with id i=85
    2local objectsFolder = model:browse():objectsFolder()
    3print(objectsFolder.Attrs.BrowseName.Name) -- "Objects"
    4print(objectsFolder.Attrs.NodeId) -- "i=85"

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_model_browse.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">browser:typesFolder()</span></span>  
Returns a node under which all the types are placed.

Returns<span class="colon">:</span>  
Browser object.

**Example:**

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Resolve path to the server type node which is a type of server object instance
    2local serverType = model:browse():typesFolder():path({
    3  "ObjectTypes",
    4  "BaseObjectType",
    5  "ServerType"
    6})
    7print(serverType.Attrs.BrowseName.Name) -- "ServerType"
    8print(serverType.Attrs.NodeId) -- "i=2253"

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_model_browse.lua" class="reference external">Full source</a>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="model.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Model methods"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="editor.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Editor">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/model/editor.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="index.html" class="reference internal">Address Space API</a>
  - <a href="model.html" class="reference internal">Model methods</a>
  - <a href="browser.html" class="reference internal">Browser</a>
  - <a href="#" class="current reference internal">Editor</a>
    - <a href="#object" class="reference internal">Object</a>
    - <a href="#objecttype" class="reference internal">ObjectType</a>
    - <a href="#variable" class="reference internal">Variable</a>
    - <a href="#variabletype" class="reference internal">VariableType</a>
    - <a href="#enum" class="reference internal">Enum</a>
    - <a href="#structure" class="reference internal">Structure</a>
    - <a href="#method" class="reference internal">Method</a>
  - <a href="import_export.html" class="reference internal">Import/Export</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Address Space API](index.html)
- Editor
- <a href="../_sources/model/editor.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="editor" class="section">

# Editor<a href="#editor" class="headerlink" title="Link to this heading"></a>

Editor is returned by <span class="pre">`model:edit()`</span>. It is used to edit the address space. It has the same interface as browser with additional methods to add new nodes to the model.

To save changes to the model you should call <span class="pre">`editor:save()`</span> method. Without saving changes are not committed to the model.

Objects returned from the following methods <span class="pre">`editor:addXXX()`</span> have the same interface as objects returned from <span class="pre">`model:browse()`</span> method plus additional methods to edit the node. All changes made to them are saved in the editor and will be committed to the model when you call <span class="pre">`editor:save()`</span> method.

Method argument and structure field tables use the same compact shape:

| Field                                      | Required | Description                                                                  |
|--------------------------------------------|----------|------------------------------------------------------------------------------|
| <span class="pre">`Name`</span>            | yes      | Argument or field name.                                                      |
| <span class="pre">`DataType`</span>        | yes      | Data type NodeId.                                                            |
| <span class="pre">`ValueRank`</span>       | optional | Value rank. If omitted for method input arguments, the default rank is used. |
| <span class="pre">`ArrayDimensions`</span> | optional | Array dimensions.                                                            |
| <span class="pre">`Description`</span>     | optional | Localized description.                                                       |

<span class="sig-name descname"><span class="pre">editor:addObject(browseName,</span> <span class="pre">objectType,</span> <span class="pre">nodeId,</span> <span class="pre">refType)</span></span>  
Add an object to the model under ObjectsFolder node.

BrowseName<span class="colon">:</span>  
<a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a> or <span class="pre">`String`</span> Name of the object.

ObjectType<span class="colon">:</span>  
<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> \| <a href="#objecttype" class="reference internal"><span class="std std-ref">ObjectType</span></a> object. Object type which will be used as a template for the new object. The hierarchy of object type will be expanded.

NodeId<span class="colon">:</span>  
Node ID for the new object. If not provided, a new Node ID is generated.

RefType<span class="colon">:</span>  
Reference type as <a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a>, <span class="pre">`String`</span>, reference type object, or <span class="pre">`nil`</span> to use HasComponent.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1  local object = editor:addObject("Sensor")
    2  object:addVariable("Temperature", {Type=ua.VariantType.Double, Value=20})

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">editor:addObjectType(browseName)</span></span>  
Add an object type to the model.

BrowseName<span class="colon">:</span>  
<a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a> or <span class="pre">`String`</span> Name of the object.

Returns<span class="colon">:</span>  
<a href="#objecttype" class="reference internal"><span class="std std-ref">ObjectType</span></a>

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Add a sensor object type. This object type will be used
    2-- to create instances of sensors.
    3local sensorObjectType = modelEditor:addObjectType("SensorType")
    4-- Add a property to the sensor object type
    5local addressProp = sensorObjectType:addProperty("Address",
    6    {Type = ua.VariantType.Int32, Value = 1})
    7
    8local objectsFolder = modelEditor:objectsFolder()
    9local sensorFolder = objectsFolder:addFolder("Sensors")

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object_type.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">editor:addVariableType(browseName)</span></span>  
Add a variable type to the model.

Returns<span class="colon">:</span>  
<a href="#variabletype" class="reference internal"><span class="std std-ref">VariableType</span></a>

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Add a variable type for the sensor data
    2-- Variables of this type will have root node with structured value and
    3-- underlying hierarchy of fields will be expanded.
    4local sensorVariableDataType = modelEditor:addVariableType("SensorVariableDataType", nil, sensorDataType)
    5-- Add a variable to the sensor object type
    6local sensorDataVariable = sensorObjectType:addVariable("Data", nil, sensorVariableDataType)
    7sensorDataVariable.Attrs.Description = {Text="Current measured sensor data"}

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object_type.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">editor:addEnum(browseName,</span> <span class="pre">values,</span> <span class="pre">nodeId)</span></span>  
Add an enum to the model.

Values<span class="colon">:</span>  
Array of string. Each value is a name of the enum value, values are assigned in the order they are defined.

Returns<span class="colon">:</span>  
<a href="#enum" class="reference internal"><span class="std std-ref">Enum</span></a>.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Add enum for the sensor data
    2local sensorDataEnum = modelEditor:addEnum("PrecisionType", {
    3  "Low",
    4  "Medium",
    5  "High",
    6})

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object_type.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">editor:addStructure(browseName,</span> <span class="pre">nodeId)</span></span>  
Add a structure to the model.

BrowseName<span class="colon">:</span>  
<a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a> or <span class="pre">`String`</span> Name of the structure.

NodeId<span class="colon">:</span>  
Node ID for the new structure. If not provided, a new Node ID is generated.

Returns<span class="colon">:</span>  
<a href="#structure" class="reference internal"><span class="std std-ref">Structure</span></a> That can be used to add fields to the structure.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Add a structure type for the sensor data
    2local sensorDataType = modelEditor:addStructure("SensorDataType")
    3sensorDataType:addField("Temperature", ua.DataTypeId.Double, ua.ValueRank.Scalar)
    4sensorDataType:addField("Humidity", ua.DataTypeId.Double, ua.ValueRank.Scalar)
    5sensorDataType:addField("Precision", ua.DataTypeId.Enumeration, ua.ValueRank.Scalar)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object_type.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">editor:save()</span></span>  
Save the model.

Returns<span class="colon">:</span>  
nil

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1local modelEditor = server.model:edit()
    2local objectsFolder = modelEditor:objectsFolder()
    3local sensorFolder = objectsFolder:addFolder("Sensors")
    4local sensor1 = sensorFolder:addObject("Outside", sensorObjectType)
    5local sensor2 = sensorFolder:addObject("Inside", sensorObjectType)
    6
    7-- Save the changes to the model
    8modelEditor:save()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object_type.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">editor:objectsFolder()</span></span>  
Returns ‘ObjectsFolder’.

Returns<span class="colon">:</span>  
<a href="#object" class="reference internal"><span class="std std-ref">Object</span></a> that can be used for editing objects under ObjectsFolder.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1local modelEditor = server.model:edit()
    2-- Get ObjectsFolder node
    3local objectsFolder = modelEditor:objectsFolder()
    4-- Create to node folders and add some variables to them
    5local folder1 = objectsFolder:addFolder("Folder1")

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_nodes.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">editor:typesFolder()</span></span>  
Returns the <span class="pre">`Types`</span> folder.

Returns<span class="colon">:</span>  
Editable browser object for the <span class="pre">`Types`</span> folder.

<!-- -->

<span class="sig-name descname"><span class="pre">editor:findNode(node)</span></span>  
Find a node by NodeId string or existing node table and return an editable object for it.

Node<span class="colon">:</span>  
NodeId string or node table.

Returns<span class="colon">:</span>  
Editable node object, or <span class="pre">`nil`</span> if the node is not found.

<!-- -->

<span class="sig-name descname"><span class="pre">editor:getNode(node)</span></span>  
Return an editable object for a NodeId string or existing node table. Raises an error if the node cannot be resolved.

Node<span class="colon">:</span>  
NodeId string or node table.

Returns<span class="colon">:</span>  
Editable node object.

<div id="object" class="section">

## Object<a href="#object" class="headerlink" title="Link to this heading"></a>

<span class="sig-name descname"><span class="pre">object:addProperty(browseName,</span> <span class="pre">value,</span> <span class="pre">variableType,</span> <span class="pre">nodeId,</span> <span class="pre">refType)</span></span>  
Add a property to the object.

BrowseName<span class="colon">:</span>  
<a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a> \| <span class="pre">`String`</span> Name of the property.

Value<span class="colon">:</span>  
(<a href="../types.html#datavalue" class="reference internal"><span class="std std-ref">DataValue</span></a> or <a href="../types.html#variant" class="reference internal"><span class="std std-ref">Variant</span></a> or nil) Value of the property.

VariableType<span class="colon">:</span>  
(optional) <a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> \| DataType. Variable type which will be used as a template for the new property.

NodeId<span class="colon">:</span>  
(optional) Node ID for the new property. If not provided, a new Node ID is generated.

RefType<span class="colon">:</span>  
(optional) Reference type of the property.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a> object.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1  local configurationObject = object:addObject("Configuration")
    2  configurationObject:addProperty("Address", {Type=ua.VariantType.String, Value="1"})
    3  configurationObject:addProperty("Port", {Type=ua.VariantType.UInt16, Value=1234})

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">object:addVariable(browseName,</span> <span class="pre">value,</span> <span class="pre">variableType,</span> <span class="pre">nodeId,</span> <span class="pre">refType)</span></span>  
Add a variable to the object.

BrowseName<span class="colon">:</span>  
(<span class="pre">`String`</span> or <a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a>) Name of the variable.

Value<span class="colon">:</span>  
(<a href="../types.html#datavalue" class="reference internal"><span class="std std-ref">DataValue</span></a> or <a href="../types.html#variant" class="reference internal"><span class="std std-ref">Variant</span></a> or nil) Value of the variable.

VariableType<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or <a href="#variabletype" class="reference internal"><span class="std std-ref">VariableType</span></a> or nil) Variable type which will be used as a template for the new variable. The hierarchy of variable type will be expanded. If not provided, then BaseDataVariableType is used.

NodeId<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or nil) Node ID for the new variable. If not provided, a new Node ID is generated.

RefType<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or <span class="pre">`String`</span> or reference type object or nil) Reference type of the variable. If not provided, then HasComponent reference type is used.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a> object.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1  local editor = model:edit()
    2
    3  local object = editor:addObject("Sensor")
    4  object:addVariable("Temperature", {Type=ua.VariantType.Double, Value=20})

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">object:addObject(browseName,</span> <span class="pre">objectType,</span> <span class="pre">nodeId,</span> <span class="pre">refType)</span></span>  
Add a sub-object to the object.

BrowseName<span class="colon">:</span>  
<a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a> or <span class="pre">`String`</span> Name of the object.

ObjectType<span class="colon">:</span>  
<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> \| <a href="#objecttype" class="reference internal"><span class="std std-ref">ObjectType</span></a> object. Object type which will be used as a template for the new object. The hierarchy of object type will be expanded.

NodeId<span class="colon">:</span>  
Node ID for the new object. If not provided, a new Node ID is generated.

RefType<span class="colon">:</span>  
Reference type as <a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a>, <span class="pre">`String`</span>, reference type object, or <span class="pre">`nil`</span> to use HasComponent.

Returns<span class="colon">:</span>  
<a href="#object" class="reference internal"><span class="std std-ref">Object</span></a>

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1  local editor = model:edit()
    2
    3  local object = editor:addObject("Sensor")
    4  object:addVariable("Temperature", {Type=ua.VariantType.Double, Value=20})
    5  local configurationObject = object:addObject("Configuration")
    6  configurationObject:addProperty("Address", {Type=ua.VariantType.String, Value="1"})

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">object:addFolder(browseName\[,</span> <span class="pre">nodeId\])</span></span>  
Add a folder which stores other nodes (like ‘ObjectsFolder’)

BrowseName<span class="colon">:</span>  
<a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a> or <span class="pre">`String`</span> Name of the folder.

NodeId<span class="colon">:</span>  
Optional Node ID for the new folder. If not provided, a new Node ID is generated.

Returns<span class="colon">:</span>  
<a href="#object" class="reference internal"><span class="std std-ref">Object</span></a> with newly created folder

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Get ObjectsFolder node
    2local objectsFolder = modelEditor:objectsFolder()
    3-- Create to node folders and add some variables to them
    4local folder1 = objectsFolder:addFolder("Folder1")

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_nodes.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">object:addMethod(browseName,</span> <span class="pre">func,</span> <span class="pre">inputArguments,</span> <span class="pre">outputArguments,</span> <span class="pre">nodeId)</span></span>  
Add a method to the Object.

Func<span class="colon">:</span>  
Function to be called when the method is called by client.

InputArguments<span class="colon">:</span>  
Array of method argument tables.

OutputArguments<span class="colon">:</span>  
Array of method argument tables.

NodeId<span class="colon">:</span>  
(optional) Node ID for the new method. If not provided, a new Node ID is generated.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

     1  local inputArguments = {
     2    {Name = "Address",  DataType = ua.DataTypeId.String},
     3    {Name = "Port",     DataType = ua.DataTypeId.UInt16},
     4    {Name = "Enabled",  DataType = ua.DataTypeId.Boolean},
     5  }
     6
     7  local outputArguments = {
     8    {Name = "Result",  DataType = ua.DataTypeId.StatusCode},
     9  }
    10
    11  local func = function(address, port, enabled)
    12    trace(string.format("New configuration: address=%s, port=%d, enabled=%s",
    13      address, port, enabled))
    14
    15    return {
    16      {Name="Result", {Type = ua.DataTypeId.StatusCode, Value = ua.StatusCode.Good}}
    17    }
    18  end
    19
    20  local method = object:addMethod("SetConfiguration",
    21    func, inputArguments, outputArguments)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object.lua" class="reference external">Full source</a>

Returns<span class="colon">:</span>  
<a href="#method" class="reference internal"><span class="std std-ref">Method</span></a>

<!-- -->

<span class="sig-name descname"><span class="pre">object:getProperty(browseName)</span></span>  
Get a property from the object.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a>

<!-- -->

<span class="sig-name descname"><span class="pre">object:getVariable(browseName)</span></span>  
Get a variable from the object.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a>

<!-- -->

<span class="sig-name descname"><span class="pre">object:getComponent(browseName)</span></span>  
Get a component from the object.

Returns<span class="colon">:</span>  
<a href="#object" class="reference internal"><span class="std std-ref">Object</span></a>

<!-- -->

<span class="sig-name descname"><span class="pre">object:getMethod(browseName)</span></span>  
Get a method from the object.

Returns<span class="colon">:</span>  
<a href="#method" class="reference internal"><span class="std std-ref">Method</span></a>

<!-- -->

<span class="sig-name descname"><span class="pre">object:path(names)</span></span>  
Resolve node by path relative to current object.

Throws<span class="colon">:</span>  
Error if path is not found.

Returns<span class="colon">:</span>  
Node corresponding to the path.

</div>

<div id="objecttype" class="section">

## ObjectType<a href="#objecttype" class="headerlink" title="Link to this heading"></a>

<span class="sig-name descname"><span class="pre">objectType:addProperty(browseName,</span> <span class="pre">value,</span> <span class="pre">variableType,</span> <span class="pre">nodeId,</span> <span class="pre">refType)</span></span>  
Add a property to the object type.

BrowseName<span class="colon">:</span>  
<a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a> \| <span class="pre">`String`</span> Name of the property.

Value<span class="colon">:</span>  
(<a href="../types.html#datavalue" class="reference internal"><span class="std std-ref">DataValue</span></a> or <a href="../types.html#variant" class="reference internal"><span class="std std-ref">Variant</span></a> or nil) Value of the property.

VariableType<span class="colon">:</span>  
(optional) <a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> \| DataType. Variable type which will be used as a template for the new property.

NodeId<span class="colon">:</span>  
(optional) Node ID for the new property. If not provided, a new Node ID is generated.

RefType<span class="colon">:</span>  
(optional) Reference type of the property.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a> object.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Add a sensor object type. This object type will be used
    2-- to create instances of sensors.
    3local sensorObjectType = modelEditor:addObjectType("SensorType")
    4-- Add a property to the sensor object type

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object_type.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">objectType:addVariable(browseName,</span> <span class="pre">value,</span> <span class="pre">variableType,</span> <span class="pre">nodeId,</span> <span class="pre">refType)</span></span>  
Add a variable to the object.

BrowseName<span class="colon">:</span>  
(<span class="pre">`String`</span> or <a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a>) Name of the variable.

Value<span class="colon">:</span>  
(<a href="../types.html#datavalue" class="reference internal"><span class="std std-ref">DataValue</span></a> or <a href="../types.html#variant" class="reference internal"><span class="std std-ref">Variant</span></a> or nil) Value of the variable. If not provided, then nil is used.

VariableType<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or <a href="#variabletype" class="reference internal"><span class="std std-ref">VariableType</span></a> or nil) Variable type which will be used as a template for the new variable. The hierarchy of variable type will be expanded. If not provided, then BaseDataVariableType is used.

NodeId<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or nil) Node ID for the new variable. If not provided, a new Node ID is generated.

RefType<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or <span class="pre">`String`</span> or reference type object or nil) Reference type of the variable. If not provided, then HasComponent reference type is used.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a> object.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Add a sensor object type. This object type will be used
    2-- to create instances of sensors.
    3local sensorObjectType = modelEditor:addObjectType("SensorType")
    4-- Add a variable to the sensor object type
    5local sensorDataVariable = sensorObjectType:addVariable("Data", nil, sensorVariableDataType)
    6sensorDataVariable.Attrs.Description = {Text="Current measured sensor data"}

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object_type.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">objectType:addObject(browseName,</span> <span class="pre">objectType,</span> <span class="pre">nodeId,</span> <span class="pre">refType)</span></span>  
Add a sub-object to the object type.

BrowseName<span class="colon">:</span>  
<a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a> or <span class="pre">`String`</span> Name of the object.

ObjectType<span class="colon">:</span>  
<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> \| <a href="#objecttype" class="reference internal"><span class="std std-ref">ObjectType</span></a> object. Object type which will be used as a template for the new object. The hierarchy of object type will be expanded.

NodeId<span class="colon">:</span>  
Node ID for the new object. If not provided, a new Node ID is generated.

RefType<span class="colon">:</span>  
Reference type as <a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a>, <span class="pre">`String`</span>, reference type object, or <span class="pre">`nil`</span> to use HasComponent.

Returns<span class="colon">:</span>  
<a href="#object" class="reference internal"><span class="std std-ref">Object</span></a>

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Add a sensor object type. This object type will be used
    2-- to create instances of sensors.
    3local sensorObjectType = modelEditor:addObjectType("SensorType")
    4-- Add an object to the sensor object type
    5local busObject = sensorObjectType:addObject("Bus")
    6busObject.Attrs.Description = {Text="Ethernet bus object"}

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object_type.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">objectType:addFolder(browseName\[,</span> <span class="pre">nodeId\])</span></span>  
Add a folder which stores other nodes (like ‘ObjectsFolder’) Parameters are the same as for <span class="pre">`object:addFolder()`</span>.

Returns<span class="colon">:</span>  
<a href="#object" class="reference internal"><span class="std std-ref">Object</span></a> with newly created folder

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Add a sensor object type. This object type will be used
    2-- Add an object to the sensor object type
    3local gpioFolder = sensorObjectType:addFolder("GPIO Pins")
    4gpioFolder:addVariable("GPIO 1", {Type = ua.VariantType.Int32, Value = 1})
    5gpioFolder:addVariable("GPIO 2", {Type = ua.VariantType.Int32, Value = 2})
    6gpioFolder:addVariable("GPIO 3", {Type = ua.VariantType.Int32, Value = 3})

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object_type.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">objectType:addMethod(browseName,</span> <span class="pre">func,</span> <span class="pre">inputArguments,</span> <span class="pre">outputArguments\[,</span> <span class="pre">nodeId\])</span></span>  
Add a method to the object type.

Func<span class="colon">:</span>  
Function to be called when the method is called by client.

InputArguments<span class="colon">:</span>  
Array of method argument tables.

OutputArguments<span class="colon">:</span>  
Array of method argument tables.

Throws<span class="colon">:</span>  
Error in case of errors.

Returns<span class="colon">:</span>  
<a href="#method" class="reference internal"><span class="std std-ref">Method</span></a>

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

     1-- Add a sensor object type. This object type will be used
     2-- to create instances of sensors.
     3local sensorObjectType = modelEditor:addObjectType("SensorType")
     4-- Add a method to the sensor object type
     5local function measureSensor(objectId, methodId, isAsync)
     6  if isAsync then
     7    return ua.StatusCode.Good
     8  end
     9
    10  return ua.StatusCode.Good
    11end
    12local inputArguments = {
    13  {Name = "IsAsync",  DataType = ua.DataTypeId.Boolean},
    14}
    15local outputArguments = {
    16  {Name = "Status",  DataType = ua.DataTypeId.StatusCode},
    17}
    18local measureMethod = sensorObjectType:addMethod("Measure",
    19    measureSensor, inputArguments, outputArguments)
    20measureMethod.Attrs.Description = {Text="Measure the temperature of the sensor"}

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_object_type.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">objectType:getProperty(browseName)</span></span>  
Get a property from the object.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a>

<!-- -->

<span class="sig-name descname"><span class="pre">objectType:getVariable(browseName)</span></span>  
Get a variable from the object.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a>

<!-- -->

<span class="sig-name descname"><span class="pre">objectType:getComponent(browseName)</span></span>  
Get a component from the object.

Returns<span class="colon">:</span>  
<a href="#object" class="reference internal"><span class="std std-ref">Object</span></a>

<!-- -->

<span class="sig-name descname"><span class="pre">objectType:getMethod(browseName)</span></span>  
Get a method from the object.

Returns<span class="colon">:</span>  
<a href="#method" class="reference internal"><span class="std std-ref">Method</span></a>

<!-- -->

<span class="sig-name descname"><span class="pre">objectType:path(names)</span></span>  
Resolve node by path relative to current object.

Returns<span class="colon">:</span>  
Node corresponding to the path.

</div>

<div id="variable" class="section">

## Variable<a href="#variable" class="headerlink" title="Link to this heading"></a>

<span class="sig-name descname"><span class="pre">variable:addVariable(browseName,</span> <span class="pre">value,</span> <span class="pre">variableType,</span> <span class="pre">nodeId,</span> <span class="pre">refType)</span></span>  
Add a variable to the object.

BrowseName<span class="colon">:</span>  
(<span class="pre">`String`</span> or <a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a>) Name of the variable.

Value<span class="colon">:</span>  
(<a href="../types.html#datavalue" class="reference internal"><span class="std std-ref">DataValue</span></a> or <a href="../types.html#variant" class="reference internal"><span class="std std-ref">Variant</span></a> or nil) Value of the variable.

VariableType<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or <a href="#variabletype" class="reference internal"><span class="std std-ref">VariableType</span></a> or nil) Variable type which will be used as a template for the new variable. The hierarchy of variable type will be expanded. If not provided, then BaseDataVariableType is used.

NodeId<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or nil) Node ID for the new variable. If not provided, a new Node ID is generated.

RefType<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or <span class="pre">`String`</span> or reference type object or nil) Reference type of the variable. If not provided, then HasComponent reference type is used.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a> object.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Add a variable for the sensor data
    2local objectsFolder = modelEditor:objectsFolder()
    3
    4local sensorDataVariable = objectsFolder:addVariable("SensorData")
    5sensorDataVariable:addVariable("Temperature",
    6    {Type = ua.VariantType.Double, Value = 20})
    7
    8sensorDataVariable:addVariable("Humidity",
    9    {Type = ua.VariantType.Double, Value = 50})

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_variable.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">variable:getVariable(browseName)</span></span>  
Get a child node variable by name.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a>

<!-- -->

<span class="sig-name descname"><span class="pre">variable:setValueCallback(func)</span></span>  
Set a callback used when the variable value is read or written.

Func<span class="colon">:</span>  
Function called by the address-space value handling code.

Returns<span class="colon">:</span>  
nil

<!-- -->

<span class="sig-name descname"><span class="pre">variable:path(names)</span></span>  
Resolve node by path relative to current variable.

Returns<span class="colon">:</span>  
Node corresponding to the path.

</div>

<div id="variabletype" class="section">

## VariableType<a href="#variabletype" class="headerlink" title="Link to this heading"></a>

Variable type is a type of variable. It is used to describe the type of variable. When creating a variable, the variable type is used as a template for the new variable. If the variable is structured, then VariableType is used as a template for the new variable and its underlying hierarchy.

For example, variable type BuildInfoType represents a structure with the following fields:

- **ProductUri**

- **ManufacturerName**

- **ProductName**

- **SoftwareVersion**

After creating a variable with type BuildInfoType, the variable will have the same fields as the variable type. The root node will have a single structured value. For convenience, the editor will expand the hierarchy with fields in the underlying hierarchy. For each field, a separate variable node will be created with the same type as the field.

- BuildInfo - Structured value

  - ProductUri - String

  - ManufacturerName - String

  - ProductName - String

  - SoftwareVersion - String

<span class="sig-name descname"><span class="pre">variableType:addVariable(browseName,</span> <span class="pre">value,</span> <span class="pre">variableType,</span> <span class="pre">nodeId,</span> <span class="pre">refType)</span></span>  
Add a variable to the object.

BrowseName<span class="colon">:</span>  
(<span class="pre">`String`</span> or <a href="../types.html#qualifiedname" class="reference internal"><span class="std std-ref">QualifiedName</span></a>) Name of the variable.

Value<span class="colon">:</span>  
(<a href="../types.html#datavalue" class="reference internal"><span class="std std-ref">DataValue</span></a> or <a href="../types.html#variant" class="reference internal"><span class="std std-ref">Variant</span></a> or nil) Value of the variable.

VariableType<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or <a href="#variabletype" class="reference internal"><span class="std std-ref">VariableType</span></a> or nil) Variable type which will be used as a template for the new variable. The hierarchy of variable type will be expanded. If not provided, then BaseDataVariableType is used.

NodeId<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or nil) Node ID for the new variable. If not provided, a new Node ID is generated.

RefType<span class="colon">:</span>  
(<a href="../types.html#nodeid" class="reference internal"><span class="std std-ref">NodeId</span></a> or <span class="pre">`String`</span> or reference type object or nil) Reference type of the variable. If not provided, then HasComponent reference type is used.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a> object.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1-- Add a variable type for the sensor data
    2-- Variables of this type will have root node with structured value and underlying hierarchy of fields will be expanded.
    3local sensorVariableDataType = modelEditor:addVariableType("SensorVariableType")
    4sensorVariableDataType:addVariable("Temperature",
    5    {Type = ua.VariantType.Double, Value = 20})
    6
    7sensorVariableDataType:addVariable("Humidity",
    8    {Type = ua.VariantType.Double, Value = 50})

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_variable_type.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">variableType:getVariable(browseName)</span></span>  
Get a child node variable by name.

Returns<span class="colon">:</span>  
<a href="#variable" class="reference internal"><span class="std std-ref">Variable</span></a>

<!-- -->

<span class="sig-name descname"><span class="pre">variableType:path(names)</span></span>  
Resolve node by path relative to current variable type.

Throws<span class="colon">:</span>  
Error if path is not found.

Returns<span class="colon">:</span>  
Node corresponding to the path relative to current variable type.

</div>

<div id="enum" class="section">

## Enum<a href="#enum" class="headerlink" title="Link to this heading"></a>

<span class="sig-name descname"><span class="pre">enum:setValues(values)</span></span>  
Set values for the enum.

Values<span class="colon">:</span>  
Array of string. Each value is a name of the enum value, values are assigned in the order they are defined.

Returns<span class="colon">:</span>  
nil

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1local values = sensorDataEnum:getValues()
    2for i, value in ipairs(values) do
    3  print(value)
    4end
    5
    6-- Add a new value to the enum
    7sensorDataEnum:setValues({"Low", "Medium", "High", "VeryHigh"})

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_enum.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">enum:getValues()</span></span>  
Get values for the enum.

Returns<span class="colon">:</span>  
Array of string. Each value is a name of the enum value, values are assigned in the order they are defined.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1local values = sensorDataEnum:getValues()
    2for i, value in ipairs(values) do
    3  print(value)
    4end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_enum.lua" class="reference external">Full source</a>

</div>

<div id="structure" class="section">

## Structure<a href="#structure" class="headerlink" title="Link to this heading"></a>

> <div>
>
> Structure is a complex data type. It is a collection of fields. Structure types describe how to encode and decode the structure. Fields are encoded in the order they are defined.
>
> </div>

<span class="sig-name descname"><span class="pre">structure:addField(browseName,</span> <span class="pre">dataType,</span> <span class="pre">rank)</span></span>  
Add one field to the structure definition.

BrowseName<span class="colon">:</span>  
Field name.

DataType<span class="colon">:</span>  
Field data type NodeId.

Rank<span class="colon">:</span>  
Optional value rank.

Returns<span class="colon">:</span>  
nil

<!-- -->

<span class="sig-name descname"><span class="pre">structure:setFields(fields)</span></span>  
Set fields for the structure.

Fields<span class="colon">:</span>  
Array of structure field tables.

Returns<span class="colon">:</span>  
nil

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

     1local fields = {
     2  {
     3    Name = "Temperature",
     4    DisplayName = {Text = "Temperature"},
     5    DataType = ua.DataTypeId.Double,
     6    ValueRank = ua.ValueRank.Scalar,
     7    IsOptional = false
     8  },
     9  {
    10    Name = "Humidity",
    11    DisplayName = {Text = "Humidity"},
    12    DataType = ua.DataTypeId.Double,
    13    ValueRank = ua.ValueRank.Scalar,
    14    IsOptional = false
    15  },
    16  {
    17    Name = "Precision",
    18    DisplayName = {Text = "Precision"},
    19    DataType = ua.DataTypeId.Enumeration,
    20    ValueRank = ua.ValueRank.Scalar,
    21    IsOptional = false
    22  },
    23}
    24sensorDataType:setFields(fields)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_structure.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">structure:getFields()</span></span>  
Get fields for the structure.

Returns<span class="colon">:</span>  
Array of structure field tables.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1local allFields = sensorDataType:getFields()
    2for _, field in ipairs(allFields) do
    3  print(field.Name, field.DisplayName, field.DataType, field.ValueRank, field.IsOptional)
    4end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_structure.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">structure:getField(fieldName)</span></span>  
Get field for the structure.

FieldName<span class="colon">:</span>  
Name of the field.

Returns<span class="colon">:</span>  
Field.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1local field = sensorDataType:getField("Precision")
    2print(field.Name, field.DisplayName, field.DataType, field.ValueRank, field.IsOptional)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_structure.lua" class="reference external">Full source</a>

</div>

<div id="method" class="section">

## Method<a href="#method" class="headerlink" title="Link to this heading"></a>

<span class="sig-name descname"><span class="pre">method:setInputArguments(inputArguments)</span></span>  
Set **input** arguments for the method.

InputArguments<span class="colon">:</span>  
Array of method argument tables.

Returns<span class="colon">:</span>  
nil

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1local inputArguments = {
    2  {Name = "IsAsync",  DataType = ua.DataTypeId.Boolean},
    3  {Name = "Unit",  DataType = ua.DataTypeId.String},
    4}
    5measureMethod:setInputArguments(inputArguments)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_method.lua" class="reference external">Full source</a>

<!-- -->

<span class="sig-name descname"><span class="pre">method:setOutputArguments(outputArguments)</span></span>  
Set **output** arguments for the method.

OutputArguments<span class="colon">:</span>  
Array of method argument tables.

Returns<span class="colon">:</span>  
nil

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    1local outputArguments = {
    2  {Name = "Status",  DataType = ua.DataTypeId.StatusCode},
    3  {Name = "Temperature",  DataType = ua.DataTypeId.Double},
    4}
    5measureMethod:setOutputArguments(outputArguments)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_add_method.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="browser.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Browser"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="import_export.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Import/Export">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/model/import_export.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="index.html" class="reference internal">Address Space API</a>
  - <a href="model.html" class="reference internal">Model methods</a>
  - <a href="browser.html" class="reference internal">Browser</a>
  - <a href="editor.html" class="reference internal">Editor</a>
  - <a href="#" class="current reference internal">Import/Export</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Address Space API](index.html)
- Import/Export
- <a href="../_sources/model/import_export.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="import-export" class="section">

# Import/Export<a href="#import-export" class="headerlink" title="Link to this heading"></a>

<span class="sig-name descname"><span class="pre">model:loadXmlModels(modelFiles)</span></span>  
Load XML models into the address space.

ModelFiles<span class="colon">:</span>  
Array of file paths, HTTP/HTTPS URLs, or XML document strings.

For every new model, a new namespace is created with the next index after the last added namespace. Namespace index 0 is reserved for the default namespace ‘<a href="http://opcfoundation.org/UA/" class="reference external">http://opcfoundation.org/UA/</a>’. If model with same URI already exists, it will be merged with the new one.

<!-- -->

<span class="sig-name descname"><span class="pre">model:exportXml(output\[,</span> <span class="pre">namespaceUris\])</span></span>  
Export XML models by calling <span class="pre">`output`</span> with XML chunks. If <span class="pre">`namespaceUris`</span> is not provided, all namespaces are exported.

Output<span class="colon">:</span>  
Callback function that receives XML string chunks.

NamespaceUris<span class="colon">:</span>  
Array of namespace URIs to export.

Returns<span class="colon">:</span>  
nil

Throws<span class="colon">:</span>  
error in case of errors.

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="editor.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Editor"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="../client/index.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Client">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/model/index.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="#" class="current reference internal">Address Space API</a>
  - <a href="model.html" class="reference internal">Model methods</a>
  - <a href="browser.html" class="reference internal">Browser</a>
  - <a href="editor.html" class="reference internal">Editor</a>
  - <a href="import_export.html" class="reference internal">Import/Export</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Address Space API
- <a href="../_sources/model/index.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="address-space-api" class="section">

# Address Space API<a href="#address-space-api" class="headerlink" title="Link to this heading"></a>

OPC UA address space is defined by OPC UA specification. The structure of the address space represents a complex type system. Managing nodes as a set of attributes and linking them by references can be difficult and error-prone. To simplify this process, the OPC UA SDK provides a model interface to simplify this process.

<div class="toctree-wrapper compound">

- <a href="model.html" class="reference internal">Model methods</a>
  - <a href="model.html#ua.emptyModel" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.emptyModel()</code></span></a>
  - <a href="model.html#ua.baseModel" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.baseModel()</code></span></a>
- <a href="browser.html" class="reference internal">Browser</a>
- <a href="editor.html" class="reference internal">Editor</a>
  - <a href="editor.html#object" class="reference internal">Object</a>
  - <a href="editor.html#objecttype" class="reference internal">ObjectType</a>
  - <a href="editor.html#variable" class="reference internal">Variable</a>
  - <a href="editor.html#variabletype" class="reference internal">VariableType</a>
  - <a href="editor.html#enum" class="reference internal">Enum</a>
  - <a href="editor.html#structure" class="reference internal">Structure</a>
  - <a href="editor.html#method" class="reference internal">Method</a>
- <a href="import_export.html" class="reference internal">Import/Export</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="../opcua_overview.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="OPC UA overview"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="model.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Model methods">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/model/model.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="index.html" class="reference internal">Address Space API</a>
  - <a href="#" class="current reference internal">Model methods</a>
    - <a href="#ua.emptyModel" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.emptyModel()</code></span></a>
    - <a href="#ua.baseModel" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.baseModel()</code></span></a>
  - <a href="browser.html" class="reference internal">Browser</a>
  - <a href="editor.html" class="reference internal">Editor</a>
  - <a href="import_export.html" class="reference internal">Import/Export</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Address Space API](index.html)
- Model methods
- <a href="../_sources/model/model.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="model-methods" class="section">

# Model methods<a href="#model-methods" class="headerlink" title="Link to this heading"></a>

<span class="sig-prename descclassname"><span class="pre">ua.</span></span><span class="sig-name descname"><span class="pre">emptyModel</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">config</span></span>*<span class="sig-paren">)</span><a href="#ua.emptyModel" class="headerlink" title="Link to this definition"></a>  
Create empty model. <span class="pre">`config`</span> is required and must include at least the application URI and logging structure expected by the OPC UA configuration loader. To use the model, load XML NodeSet files and create the namespaces required by your application.

<!-- -->

<span class="sig-prename descclassname"><span class="pre">ua.</span></span><span class="sig-name descname"><span class="pre">baseModel</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">config</span></span>*<span class="sig-paren">)</span><a href="#ua.baseModel" class="headerlink" title="Link to this definition"></a>  
Create model with NS0 and NS1 initialized. Built-in NS0 contains a minimal set of nodes required for operating. To get the full standard nodeset NS0, you can load it from XML file: nodes will be merged. To create a full nodeset from the beginning, you can use <a href="#ua.emptyModel" class="reference internal" title="ua.emptyModel"><span class="pre"><code class="sourceCode python">ua.emptyModel()</code></span></a> and then load the required models.

<!-- -->

<span class="sig-name descname"><span class="pre">model:newNodeId()</span></span>  
Generates a <a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>. This method is used to generate new node identifiers for all new nodes created in the model. If you want to generate your own node identifiers, you can replace this method with your own implementation.

Returns<span class="colon">:</span>  
<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> with new node identifier.

<!-- -->

<span class="sig-name descname"><span class="pre">model:createNamespace(namespaceUri)</span></span>  
Create a new namespace. If a namespace with the same URI already exists, an error will be thrown. Return index of the new namespace. For every new namespace, the index will be incremented by 1. Namespace index 0 is reserved for the default namespace ‘<a href="http://opcfoundation.org/UA/" class="reference external">http://opcfoundation.org/UA/</a>’ and index 1 is reserved for the namespace reserved for the current server. All newly created namespaces will be mapped to namespaces with indexes starting from 2.

NamespaceUri<span class="colon">:</span>  
URI of the namespace.

Throws<span class="colon">:</span>  
Error if namespace with same URI already exists.

Returns<span class="colon">:</span>  
index of the new namespace.

<!-- -->

<span class="sig-name descname"><span class="pre">model:validate(callback)</span></span>  
Validate the model for errors: Missing references, unreferenced nodes, etc.

Callback<span class="colon">:</span>  
Function **callback(error)** to be called when the validation is done. Error is string with error message.

Returns<span class="colon">:</span>  
nil

Throws<span class="colon">:</span>  
Error if there are errors in the model.

<!-- -->

<span class="sig-name descname"><span class="pre">model:commit()</span></span>  
Returns<span class="colon">:</span>  
nil

Throws<span class="colon">:</span>  
Error if there are errors in the model.

Commit the model changes. This method should be called after all changes are made to the model. It validates changes, fills internal structures and makes the model ready for use.

<!-- -->

<span class="sig-name descname"><span class="pre">model:browse(\[parentNodeId\])</span></span>  
Browse the address space. If <span class="pre">`parentNodeId`</span> is omitted, browsing starts at the model root used by the browser implementation.

ParentNodeId<span class="colon">:</span>  
Optional node identifier to use as the browser root.

Returns<span class="colon">:</span>  
<a href="browser.html#browser" class="reference internal"><span class="std std-ref">Browser</span></a> object.

<!-- -->

<span class="sig-name descname"><span class="pre">model:edit()</span></span>  
Create editor to edit the address space. Every editor stores new nodes that are not yet committed to the model. To commit changes to the model, call <span class="pre">`editor:save()`</span>.

Returns<span class="colon">:</span>  
<a href="editor.html#editor" class="reference internal"><span class="std std-ref">Editor</span></a> object.

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="index.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Address Space API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="browser.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Browser">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/opcua_overview.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="about.html" class="reference internal">About opcua-lua</a>
- <a href="#" class="current reference internal">OPC UA overview</a>
  - <a href="#the-address-space" class="reference internal">The address space</a>
  - <a href="#types-and-instances" class="reference internal">Types and instances</a>
  - <a href="#common-client-server-services" class="reference internal">Common Client/Server services</a>
  - <a href="#why-it-feels-complex" class="reference internal">Why it feels complex</a>
  - <a href="#practical-mental-model" class="reference internal">Practical mental model</a>
- <a href="model/index.html" class="reference internal">Address Space API</a>
- <a href="client/index.html" class="reference internal">Client</a>
- <a href="server/index.html" class="reference internal">Server</a>
- <a href="pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="integrations/index.html" class="reference internal">Examples</a>
- <a href="logging.html" class="reference internal">Logging subsystem</a>
- <a href="secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="types.html" class="reference internal">Data Types</a>
- <a href="interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="index.html" class="icon icon-home" aria-label="Home"></a>
- OPC UA overview
- <a href="_sources/opcua_overview.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="opc-ua-overview" class="section">

# OPC UA overview<a href="#opc-ua-overview" class="headerlink" title="Link to this heading"></a>

OPC UA can look overwhelming because it is more than a wire protocol. The most important idea is that OPC UA describes an information model and then provides services for accessing that model.

<div id="the-address-space" class="section">

## The address space<a href="#the-address-space" class="headerlink" title="Link to this heading"></a>

The center of OPC UA is the address space. It is a graph of nodes that describe devices, variables, methods, data types, and relationships. A client normally does not start by knowing every value path in advance. It can browse the address space, discover what the server exposes, and then read, write, or call methods on specific nodes.

Important node concepts:

**Node**  
One item in the address space, such as an object, variable, method, data type, or reference type.

**NodeId**  
Stable identifier used by services such as Browse, Read, Write, and Call.

**BrowseName**  
Name used while browsing from one node to another.

**Reference**  
Relationship between nodes, such as <span class="pre">`HasComponent`</span> or <span class="pre">`HasProperty`</span>.

**Namespace**  
A URI-indexed naming scope that prevents models from different vendors or companion specifications from colliding.

</div>

<div id="types-and-instances" class="section">

## Types and instances<a href="#types-and-instances" class="headerlink" title="Link to this heading"></a>

OPC UA has an object-oriented type system. A model can define reusable types and then instantiate objects from those types.

| Type         | Purpose                                                                      |
|--------------|------------------------------------------------------------------------------|
| ObjectType   | Template for objects. It can define child variables, methods, and structure. |
| VariableType | Template for variables and structured variable layouts.                      |
| DataType     | Type definition for scalar values, structures, and enumerations.             |
| Structure    | Ordered set of named fields.                                                 |
| Enumeration  | Named integer values.                                                        |
| Method       | Callable operation exposed by an object.                                     |

This is why OPC UA is useful in industrial systems: a value can carry context. The server can describe what the value means, where it belongs, what type it has, and what operations are available.

</div>

<div id="common-client-server-services" class="section">

## Common Client/Server services<a href="#common-client-server-services" class="headerlink" title="Link to this heading"></a>

The Client/Server model uses request/response services.

| Service           | Purpose                                                          |
|-------------------|------------------------------------------------------------------|
| FindServers       | Discover servers known to a discovery endpoint.                  |
| GetEndpoints      | Discover endpoint URLs, security modes, and user token policies. |
| OpenSecureChannel | Establish the secure transport channel.                          |
| CreateSession     | Create a logical client session.                                 |
| ActivateSession   | Authenticate the user for the session.                           |
| Browse            | Discover nodes and references in the address space.              |
| Read              | Read node attributes, commonly variable values.                  |
| Write             | Write node attributes.                                           |
| AddNodes          | Add nodes to the address space.                                  |
| Call              | Invoke a method node.                                            |

</div>

<div id="why-it-feels-complex" class="section">

## Why it feels complex<a href="#why-it-feels-complex" class="headerlink" title="Link to this heading"></a>

OPC UA has several layers that interact:

- The information model describes the address space.

- The service model defines operations such as Browse and Read.

- The security model defines certificates, policies, users, and sessions.

- The encoding layer defines binary, JSON, XML, and PubSub message formats.

- Companion specifications define vendor-neutral models for industries and device types.

Most application code only needs a subset of this. A simple server may create objects and variables, set callbacks for values, and support anonymous access. A more complete server may load NodeSet2 XML files, expose methods, configure security policies, and validate user tokens.

</div>

<div id="practical-mental-model" class="section">

## Practical mental model<a href="#practical-mental-model" class="headerlink" title="Link to this heading"></a>

Use this sequence when learning the stack:

1.  Start with the address space: objects, variables, methods, and NodeIds.

2.  Learn Browse, Read, and Write.

3.  Add sessions and authentication.

4.  Add custom types only when your data needs structure and semantics.

5.  Use PubSub over MQTT when you need message distribution rather than a browseable address space.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="about.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="About opcua-lua"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="model/index.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Address Space API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/pubsub/client_server_vs_pubsub.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="index.html" class="reference internal">Publish Subscribe API</a>
  - <a href="#" class="current reference internal">Client/Server vs PubSub over MQTT</a>
    - <a href="#opc-ua-client-server" class="reference internal">OPC UA Client/Server</a>
    - <a href="#opc-ua-pubsub-over-mqtt" class="reference internal">OPC UA PubSub over MQTT</a>
    - <a href="#comparison" class="reference internal">Comparison</a>
    - <a href="#choosing-the-right-model" class="reference internal">Choosing the right model</a>
  - <a href="mqtt.html" class="reference internal">MQTT PubSub API</a>
  - <a href="mqtt_examples.html" class="reference internal">MQTT PubSub Examples</a>
  - <a href="mqtt_message_format.html" class="reference internal">UADP Message</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Publish Subscribe API](index.html)
- Client/Server vs PubSub over MQTT
- <a href="../_sources/pubsub/client_server_vs_pubsub.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="client-server-vs-pubsub-over-mqtt" class="section">

# Client/Server vs PubSub over MQTT<a href="#client-server-vs-pubsub-over-mqtt" class="headerlink" title="Link to this heading"></a>

OPC UA Client/Server and OPC UA PubSub over MQTT solve different problems. They share OPC UA data concepts, but they do not expose the same interaction model.

<figure id="id1" class="align-default">
<img src="../_images/opc-ua-vs-pubsub-opc-ua.jpeg" alt="OPC UA Client/Server compared with OPC UA PubSub over MQTT" />
<figcaption><p><span class="caption-text">OPC UA Client/Server exposes an address space. OPC UA PubSub over MQTT publishes messages.</span><a href="#id1" class="headerlink" title="Link to this image"></a></p></figcaption>
</figure>

<div id="opc-ua-client-server" class="section">

## OPC UA Client/Server<a href="#opc-ua-client-server" class="headerlink" title="Link to this heading"></a>

In the standard Client/Server model, OPC UA is primarily an information modeling system. The server exposes an address space, and clients interact with that address space through services.

The address space can contain:

- ObjectTypes and objects.

- VariableTypes and variables.

- Structures and enumerations.

- Methods that clients can call.

- References that describe relationships between nodes.

Clients use NodeIds and browse paths to discover and access data. This makes Client/Server a good fit when the client needs context, structure, metadata, type definitions, and remote method calls.

</div>

<div id="opc-ua-pubsub-over-mqtt" class="section">

## OPC UA PubSub over MQTT<a href="#opc-ua-pubsub-over-mqtt" class="headerlink" title="Link to this heading"></a>

PubSub over MQTT is message-oriented. A publisher creates dataset messages and sends them through an MQTT broker. A subscriber receives messages and decodes the payload.

At the application level, a PubSub message is closer to a Lua table, JSON object, or dictionary than to a browseable address space. Metadata can describe the fields, but subscribers do not browse a live server address space, create sessions, or call methods through MQTT PubSub.

This makes PubSub a good fit for distributing current values, telemetry, events, or sensor data to many consumers.

</div>

<div id="comparison" class="section">

## Comparison<a href="#comparison" class="headerlink" title="Link to this heading"></a>

**Primary model**  
Client/Server exposes a browseable OPC UA address space. PubSub over MQTT publishes dataset messages.

**Communication pattern**  
Client/Server uses request/response between a client and server. PubSub uses publish/subscribe through an MQTT broker.

**Discovery**  
Client/Server clients browse nodes and references. PubSub subscribers know topics and decode messages.

**Identity**  
Client/Server is NodeId-centric. PubSub is field-, topic-, and message-centric.

**Methods**  
Client/Server supports remote method calls through the Call service. Methods are not part of the PubSub message flow.

**Best for**  
Client/Server fits rich device models, semantic data, remote operations, and integration with OPC UA clients. PubSub fits telemetry, fan-out distribution, cloud ingestion, and simple sensor data streams.

</div>

<div id="choosing-the-right-model" class="section">

## Choosing the right model<a href="#choosing-the-right-model" class="headerlink" title="Link to this heading"></a>

Use Client/Server when:

- Clients need to browse and understand the device model.

- Data should carry strong OPC UA semantics.

- The application needs Read, Write, Browse, or Call services.

- You need sessions, user authentication, and endpoint security negotiation.

Use PubSub over MQTT when:

- The goal is to publish values or events to many receivers.

- MQTT infrastructure is already part of the system.

- Subscribers do not need to browse a live OPC UA address space.

- The data can be represented as fields in a message.

Many systems use both. A device or gateway can expose a full OPC UA server for engineering tools and local control, while also publishing selected values over MQTT for dashboards, cloud services, or data pipelines.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="index.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Publish Subscribe API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="mqtt.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="MQTT PubSub API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/pubsub/index.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="#" class="current reference internal">Publish Subscribe API</a>
  - <a href="client_server_vs_pubsub.html" class="reference internal">Client/Server vs PubSub over MQTT</a>
  - <a href="mqtt.html" class="reference internal">MQTT PubSub API</a>
  - <a href="mqtt_examples.html" class="reference internal">MQTT PubSub Examples</a>
  - <a href="mqtt_message_format.html" class="reference internal">UADP Message</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Publish Subscribe API
- <a href="../_sources/pubsub/index.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="publish-subscribe-api" class="section">

# Publish Subscribe API<a href="#publish-subscribe-api" class="headerlink" title="Link to this heading"></a>

<div class="toctree-wrapper compound">

- <a href="client_server_vs_pubsub.html" class="reference internal">Client/Server vs PubSub over MQTT</a>
  - <a href="client_server_vs_pubsub.html#opc-ua-client-server" class="reference internal">OPC UA Client/Server</a>
  - <a href="client_server_vs_pubsub.html#opc-ua-pubsub-over-mqtt" class="reference internal">OPC UA PubSub over MQTT</a>
  - <a href="client_server_vs_pubsub.html#comparison" class="reference internal">Comparison</a>
  - <a href="client_server_vs_pubsub.html#choosing-the-right-model" class="reference internal">Choosing the right model</a>
- <a href="mqtt.html" class="reference internal">MQTT PubSub API</a>
  - <a href="mqtt.html#mqtt-broker-selection" class="reference internal">MQTT broker selection</a>
  - <a href="mqtt.html#mqtt-client-constructor" class="reference internal">MQTT Client constructor</a>
    - <a href="mqtt.html#ua.newMqttClient" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newMqttClient()</code></span></a>
  - <a href="mqtt.html#mqtt-connect" class="reference internal">MQTT Connect</a>
  - <a href="mqtt.html#mqtt-subscribe" class="reference internal">MQTT subscribe</a>
    - <a href="mqtt.html#messageCallback" class="reference internal"><span class="pre"><code class="docutils literal notranslate">messageCallback()</code></span></a>
  - <a href="mqtt.html#mqtt-publishing" class="reference internal">MQTT publishing</a>
- <a href="mqtt_examples.html" class="reference internal">MQTT PubSub Examples</a>
  - <a href="mqtt_examples.html#monitoring-opc-ua-server-changes" class="reference internal">Monitoring OPC UA server changes</a>
  - <a href="mqtt_examples.html#publishing-data-to-mqtt-broker" class="reference internal">Publishing data to MQTT broker</a>
  - <a href="mqtt_examples.html#subscribing-to-mqtt-messages" class="reference internal">Subscribing to MQTT messages</a>
- <a href="mqtt_message_format.html" class="reference internal">UADP Message</a>
  - <a href="mqtt_message_format.html#common-fields" class="reference internal">Common Fields</a>
    - <a href="mqtt_message_format.html#groupheader" class="reference internal">GroupHeader</a>
    - <a href="mqtt_message_format.html#securityheader" class="reference internal">SecurityHeader</a>
  - <a href="mqtt_message_format.html#uadp-datasetmessage" class="reference internal">UADP DataSetMessage</a>
  - <a href="mqtt_message_format.html#uadp-announcementmessage" class="reference internal">UADP AnnouncementMessage</a>
    - <a href="mqtt_message_format.html#dataset-metadata" class="reference internal">DATASET_METADATA</a>
    - <a href="mqtt_message_format.html#metadata" class="reference internal">Metadata</a>
    - <a href="mqtt_message_format.html#metadata-fields" class="reference internal">Metadata Fields</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="../server/server_api.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Server API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="client_server_vs_pubsub.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Client/Server vs PubSub over MQTT">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/pubsub/mqtt.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="index.html" class="reference internal">Publish Subscribe API</a>
  - <a href="client_server_vs_pubsub.html" class="reference internal">Client/Server vs PubSub over MQTT</a>
  - <a href="#" class="current reference internal">MQTT PubSub API</a>
    - <a href="#mqtt-broker-selection" class="reference internal">MQTT broker selection</a>
    - <a href="#mqtt-client-constructor" class="reference internal">MQTT Client constructor</a>
      - <a href="#ua.newMqttClient" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newMqttClient()</code></span></a>
    - <a href="#mqtt-connect" class="reference internal">MQTT Connect</a>
    - <a href="#mqtt-subscribe" class="reference internal">MQTT subscribe</a>
      - <a href="#messageCallback" class="reference internal"><span class="pre"><code class="docutils literal notranslate">messageCallback()</code></span></a>
    - <a href="#mqtt-publishing" class="reference internal">MQTT publishing</a>
  - <a href="mqtt_examples.html" class="reference internal">MQTT PubSub Examples</a>
  - <a href="mqtt_message_format.html" class="reference internal">UADP Message</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Publish Subscribe API](index.html)
- MQTT PubSub API
- <a href="../_sources/pubsub/mqtt.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="mqtt-pubsub-api" class="section">

# MQTT PubSub API<a href="#mqtt-pubsub-api" class="headerlink" title="Link to this heading"></a>

The MQTT PubSub client is used to connect to an MQTT broker, subscribe to topics, and publish data. It can be used in combination with an OPC UA server to listen to changes in nodes and publish data to an MQTT broker. It is also possible to use it to publish data to an MQTT broker without an OPC UA server. In this case, you’ll need to set data manually.

<div id="mqtt-broker-selection" class="section">

## MQTT broker selection<a href="#mqtt-broker-selection" class="headerlink" title="Link to this heading"></a>

OPC UA PubSub over MQTT does not require a specific MQTT broker. The <span class="pre">`endpointUrl`</span> form of <span class="pre">`uaMqtt:connect()`</span> can connect to any MQTT broker that is reachable by the underlying MQTT client, for example a public broker, Mosquitto, HiveMQ, EMQX, a cloud broker, or an on-premises broker.

Real Time Logic also provides an optional Lua MQTT broker for BAS-derived runtimes: <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/MQTT-Broker" class="reference external">LMQTT Broker</a>. This broker is useful when a <a href="https://makoserver.net/" class="reference external">Mako Server</a>, <a href="https://realtimelogic.com/products/xedge/" class="reference external">Xedge</a>, <a href="https://realtimelogic.com/downloads/bas/ESP32/?bas=" class="reference external">Xedge32</a>, or embedded BAS application should host MQTT locally instead of depending on an external broker.

When using <a href="https://makoserver.net/" class="reference external">Mako Server</a> for development, the recommended setup is the <a href="https://makoserver.net/documentation/developer-package/" class="reference external">mako.zip Developer Edition</a>, which includes the Lua MQTT broker module.

The broker also provides an in-process client API. Its <span class="pre">`broker:createClient()`</span> method returns an MQTT-client-like object that can be passed directly to <span class="pre">`uaMqtt:connect(mqttClient,`</span>` `<span class="pre">`transportProfileUri)`</span>. This is useful for local OPC UA PubSub publishers because messages can be routed through the broker without opening a loopback TCP connection.

</div>

<div id="mqtt-client-constructor" class="section">

## MQTT Client constructor<a href="#mqtt-client-constructor" class="headerlink" title="Link to this heading"></a>

<span class="sig-prename descclassname"><span class="pre">ua.</span></span><span class="sig-name descname"><span class="pre">newMqttClient</span></span><span class="sig-paren">(</span><span class="sig-paren">)</span><a href="#ua.newMqttClient" class="headerlink" title="Link to this definition"></a>  
<span class="sig-prename descclassname"><span class="pre">ua.</span></span><span class="sig-name descname"><span class="pre">newMqttClient</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">config</span></span>*<span class="sig-paren">)</span>  
<span class="sig-prename descclassname"><span class="pre">ua.</span></span><span class="sig-name descname"><span class="pre">newMqttClient</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">config</span></span>*, *<span class="n"><span class="pre">uaServer</span></span>*<span class="sig-paren">)</span>  
<span class="sig-prename descclassname"><span class="pre">ua.</span></span><span class="sig-name descname"><span class="pre">newMqttClient</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">config</span></span>*, *<span class="n"><span class="pre">model</span></span>*<span class="sig-paren">)</span>  
Create an instance of MQTT client. MQTT client is used to connect to MQTT broker, subscribe to topics, publish data.

Parameters<span class="colon">:</span>  
- **config** – MQTT client configuration. If not provided, then default configuration will be used.

- **uaServer** – OPC UA server instance. If provided, MQTT client can connect to nodes and listen value changes.

- **model** – OPC UA model. If provided, it will be used for encoding and decoding data structures.

Returns<span class="colon">:</span>  
MQTT client instance.

If neither <span class="pre">`uaServer`</span> nor <span class="pre">`model`</span> is provided, the MQTT client loads only the base OPC UA model. In that case, it can encode and decode only base OPC UA data types.

</div>

<div id="mqtt-connect" class="section">

## MQTT Connect<a href="#mqtt-connect" class="headerlink" title="Link to this heading"></a>

<span class="sig-name descname"><span class="pre">uaMqtt:connect(endpointUrl\[,</span> <span class="pre">transportProfileUri\]\[,</span> <span class="pre">connectCallback\]\[,</span> <span class="pre">mqttc\])</span></span>  
<span class="sig-name descname"><span class="pre">uaMqtt:connect(mqttClient,</span> <span class="pre">transportProfileUri\[,</span> <span class="pre">connectCallback\])</span></span>  
Connect to MQTT broker.

Parameters<span class="colon">:</span>  
- **endpointUrl** – MQTT endpoint URL, for example <span class="pre">`mqtt://localhost:1883`</span>.

- **mqttClient** – Existing MQTT client instance. If provided, this instance is used instead of creating a new client from <span class="pre">`endpointUrl`</span>. The object must provide the MQTT client methods used by this API, such as <span class="pre">`publish()`</span>, <span class="pre">`subscribe()`</span>, and <span class="pre">`close()`</span>. This includes the in-process client returned by the LMQTT Broker <span class="pre">`broker:createClient()`</span> method.

- **transportProfileUri** – OPC UA Transport Profile URI. Defines message encoding. Use <span class="pre">`ua.TranportProfileUri.MqttBinary`</span> or <span class="pre">`ua.TranportProfileUri.MqttJson`</span>.

- **connectCallback** – Callback function

- **mqttc** – Optional MQTT module/table used when <span class="pre">`endpointUrl`</span> is a string.

If <span class="pre">`transportProfileUri`</span> is omitted, received messages are decoded by inspecting the payload.

The <span class="pre">`endpointUrl`</span> form is the normal choice when connecting over the network to an MQTT broker. The <span class="pre">`mqttClient`</span> form is for advanced cases where the application already created an MQTT client object. With LMQTT Broker, this allows an OPC UA PubSub publisher to use a broker-local client:

When using the <span class="pre">`mqttClient`</span> form, <span class="pre">`transportProfileUri`</span> is required. Use a separate OPC UA MQTT client for each transport profile when subscribing to both JSON and binary/UADP messages through existing MQTT client objects.

<div class="highlight-lua notranslate">

<div class="highlight">

    local function connectCallback(status)
      print("Connected to MQTT broker")
    end

    uaMqtt:connect("mqtt://localhost:1883", ua.TranportProfileUri.MqttBinary, connectCallback)

</div>

</div>

<div class="highlight-lua notranslate">

<div class="highlight">

    local mqttc = require("mqttc")

    local onstatus = function(...)
    end

    local onpublish = function(topic,payload,properties,cpt)
    end

    local mqttClient = mqttc.create("mosquitto.org", onstatus, onpublish, {port=1883})
    uaMqtt:connect(mqttClient, ua.TranportProfileUri.MqttBinary)

</div>

</div>

<div class="highlight-lua notranslate">

<div class="highlight">

    local mqttbroker = require("mqttbroker")
    local broker = assert(mqttbroker.create(1883))

    local uaMqtt = ua.newMqttClient(config)
    local datasetId = uaMqtt:createDataset({
      { name = "Temperature" }
    })

    uaMqtt:connect(broker:createClient(), ua.TranportProfileUri.MqttJson)
    uaMqtt:setValue(datasetId, "Temperature", {
      Type = ua.VariantType.Double,
      Value = 21.5
    })
    uaMqtt:publish("factory/line1/data", "line1")

</div>

</div>

</div>

<div id="mqtt-subscribe" class="section">

## MQTT subscribe<a href="#mqtt-subscribe" class="headerlink" title="Link to this heading"></a>

<span class="sig-name descname"><span class="pre">uaMqtt:subscribe(topic,</span> <span class="pre">messageCallback)</span></span>  
Subscribe to MQTT topic.

Parameters<span class="colon">:</span>  
- **topic** – MQTT topic

- **messageCallback** – Callback function

<!-- -->

<span class="sig-name descname"><span class="pre">messageCallback</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">message</span></span>*, *<span class="n"><span class="pre">err</span></span>*<span class="sig-paren">)</span><a href="#messageCallback" class="headerlink" title="Link to this definition"></a>  
Callback function for MQTT message.

Parameters<span class="colon">:</span>  
- **message** – Decoded OPC UA message.

- **err** – Decode error, or <span class="pre">`nil`</span> when decoding succeeded.

</div>

<div id="mqtt-publishing" class="section">

## MQTT publishing<a href="#mqtt-publishing" class="headerlink" title="Link to this heading"></a>

<span class="sig-name descname"><span class="pre">uaMqtt:createDataset(fields\[,</span> <span class="pre">classId\])</span></span>  
Create dataset which will describe message content to publish.

Parameters<span class="colon">:</span>  
**fields** – Array of field tables. The order of fields is preserved in published MQTT payload messages.

| Field                             | Type             | Description                                           |
|-----------------------------------|------------------|-------------------------------------------------------|
| <span class="pre">`name`</span>   | string           | Field name used in JSON payloads and binary metadata. |
| <span class="pre">`nodeId`</span> | NodeId, optional | OPC UA server node whose value changes are published. |

Parameters<span class="colon">:</span>  
**classId** – Dataset class ID. If omitted, a GUID is generated. The method returns the class ID used for the dataset.

<!-- -->

<span class="sig-name descname"><span class="pre">uaMqtt:setValue(classId,</span> <span class="pre">id,</span> <span class="pre">dataValue)</span></span>  
Set field value in the dataset.

Parameters<span class="colon">:</span>  
- **classId** – Dataset class ID returned by <span class="pre">`uaMqtt:createDataset()`</span>.

- **id** – Field identifier. This can be a field name, node ID, or field index.

- **dataValue** – Field value.

<!-- -->

<span class="sig-name descname"><span class="pre">uaMqtt:publish(dataTopic,</span> <span class="pre">publisherId)</span></span>  
Publish dataset. Will forcibly publish dataset with all the changed fields.

Parameters<span class="colon">:</span>  
- **dataTopic** – Data topic

- **publisherId** – Publisher ID

<!-- -->

<span class="sig-name descname"><span class="pre">uaMqtt:startPublishing(dataTopic,</span> <span class="pre">publisherId,</span> <span class="pre">periodMs)</span></span>  
Start publishing dataset periodically with changed fields.

Parameters<span class="colon">:</span>  
- **dataTopic** – Data topic

- **publisherId** – Publisher ID. string or numeric value.

- **periodMs** – Period in milliseconds

<!-- -->

<span class="sig-name descname"><span class="pre">uaMqtt:stopPublishing()</span></span>  
Stop publishing dataset.

<!-- -->

<span class="sig-name descname"><span class="pre">uaMqtt:close()</span></span>  
Stop publishing and close the underlying MQTT client, if one is connected.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="client_server_vs_pubsub.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Client/Server vs PubSub over MQTT"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="mqtt_examples.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="MQTT PubSub Examples">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/pubsub/mqtt_examples.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="index.html" class="reference internal">Publish Subscribe API</a>
  - <a href="client_server_vs_pubsub.html" class="reference internal">Client/Server vs PubSub over MQTT</a>
  - <a href="mqtt.html" class="reference internal">MQTT PubSub API</a>
  - <a href="#" class="current reference internal">MQTT PubSub Examples</a>
    - <a href="#monitoring-opc-ua-server-changes" class="reference internal">Monitoring OPC UA server changes</a>
    - <a href="#publishing-data-to-mqtt-broker" class="reference internal">Publishing data to MQTT broker</a>
    - <a href="#subscribing-to-mqtt-messages" class="reference internal">Subscribing to MQTT messages</a>
  - <a href="mqtt_message_format.html" class="reference internal">UADP Message</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Publish Subscribe API](index.html)
- MQTT PubSub Examples
- <a href="../_sources/pubsub/mqtt_examples.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="mqtt-pubsub-examples" class="section">

# MQTT PubSub Examples<a href="#mqtt-pubsub-examples" class="headerlink" title="Link to this heading"></a>

These examples are standalone PubSub API recipes. They use a local <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/MQTT-Broker" class="reference external">MQTT broker</a> so they can run without an external broker such as Mosquitto.

The easiest setup is the <a href="https://makoserver.net/documentation/developer-package/" class="reference external">Mako Server mako.zip Developer Edition</a>, which includes the broker module used by these examples.

Run them from the <span class="pre">`LSP-Examples/OPC-UA/pubsub`</span> directory:

<div class="highlight-bash notranslate">

<div class="highlight">

    mako mqtt_publish_node_subscribe.lua
    mako mqtt_publish_serverless.lua
    mako mqtt_subscribe.lua

</div>

</div>

The examples stop immediately with a clear error message if the <span class="pre">`mqttbroker`</span> module cannot be loaded.

If you are learning OPC UA PubSub for the first time, start with <a href="../integrations/learning_examples.html" class="reference internal"><span class="doc">Learning OPC UA with Mako Server</span></a>. The examples below are more compact API-focused recipes.

<div id="monitoring-opc-ua-server-changes" class="section">

## Monitoring OPC UA server changes<a href="#monitoring-opc-ua-server-changes" class="headerlink" title="Link to this heading"></a>

It is possible to connect an MQTT PubSub publisher to an OPC UA server and publish node value changes. This example creates a local OPC UA server, creates a local MQTT broker, publishes a server node value through MQTT PubSub, and verifies the decoded subscriber message.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    -- create server
    local uaServer = ua.newServer()
    uaServer:initialize()

    local ObjectsFolder = "i=85"
    local int32DataValue = {
      Type = ua.VariantType.UInt32,
      Value = 10
    }
    local request = {
      NodesToAdd = {ua.newVariableParams(ObjectsFolder, "writeHook", int32DataValue)}
    }

    -- Add a node
    local resp = uaServer:addNodes(request)
    local results = resp.Results
    assert(results[1].StatusCode, ua.StatusCode.Good)
    local nodeId =  results[1].AddedNodeId

    -- Create MQTT client
    local config = {
      bufSize = 128 -- max size of MQTT message
    }

    local uaMmqtt = ua.newMqttClient(config, uaServer)

    -- Array with fields parameters.
    local fields = {
      -- #1
      {
        nodeId = nodeId,   -- ID of a node, which changes will be monitored
        name = "MqttNode", -- Name of a field, will as a field in JSON
      }
    }

    -- create dataset with fields
    local classId = "5fa38ebb-44d2-a3ec-d251-1030c777f10a"
    uaMmqtt:createDataset(fields, classId)

    -- Connect to MQTT broker
    local tranportProfileUri = ua.TranportProfileUri.MqttJson
    local endpointUrl = "opc.mqtt://test.mosquitto.org:1883"
    uaMmqtt:connect(endpointUrl, tranportProfileUri)

    -- Start periodic publishing
    local dataTopic = "rtl/json/data/urn:arykovanov-note:opcua:server/group/dataset"
    uaMmqtt:startPublishing(dataTopic, "test_cyclic_publisher", 2000)

    -- Run server.
    uaServer:run()

    -- Function which will periodically write data to address space
    -- Those changes will be hooked by MQTT client for publishing data.
    local writeRequest = {
      NodesToWrite = {
        {
          NodeId = nodeId,
          AttributeId = ua.AttributeId.Value,
          Value = {
            Type = ua.VariantType.UInt32,
            Value=123
          }
        }
      }
    }


    uaServer:write(writeRequest)

    uaMmqtt:stopPublishing()
    uaServer:shutdown()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/pubsub/mqtt_publish_node_subscribe.lua" class="reference external">Full source</a>

</div>

<div id="publishing-data-to-mqtt-broker" class="section">

## Publishing data to MQTT broker<a href="#publishing-data-to-mqtt-broker" class="headerlink" title="Link to this heading"></a>

It is possible to publish data to an MQTT broker without an OPC UA server. To do this, create an MQTT PubSub client, configure fields for messages, connect it to the local MQTT broker, and publish manually supplied values.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local uaMqtt = ua.newMqttClient()

    local fields = {
      { name = "Value1" },
      { name = "Value2" }
    }

    local tranportProfileUri = ua.TranportProfileUri.MqttBinary
    local endpointUrl = "opc.mqtt://test.mosquitto.org:1883"
    uaMqtt:connect(endpointUrl, tranportProfileUri)

    local datasetId = uaMqtt:createDataset(fields)

    local dataTopic = "rtl/uadp/data/urn:arykovanov-note:opcua:server/group/dataset"
    local publisherId = "test_manual_publisher"

    for i=64, 74 do
      uaMqtt:setValue(datasetId, "Value1", {Type=ua.VariantType.UInt32, Value=i})
      uaMqtt:setValue(datasetId, "Value2", {Type=ua.VariantType.UInt32, Value=i*2})
      uaMqtt:publish(dataTopic, publisherId)

      ba.sleep(1000)
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/pubsub/mqtt_publish_serverless.lua" class="reference external">Full source</a>

</div>

<div id="subscribing-to-mqtt-messages" class="section">

## Subscribing to MQTT messages<a href="#subscribing-to-mqtt-messages" class="headerlink" title="Link to this heading"></a>

OPC UA PubSub messages can be received and decoded by OPC UA MQTT client. This example starts a local broker, publishes one JSON message and one binary UADP message, and subscribes to both. When an existing MQTT client object is passed to <span class="pre">`uaMqtt:connect(...)`</span>, the transport profile must be explicit, so the example uses one JSON subscriber and one binary subscriber.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    -- Create MQTT client instance
    local mqttClient = ua.newMqttClient()

    -- Connect to MQTT broker
    local function callbackCallback(status)
      ua.printTable("status", status)
    end
    mqttClient:connect("opc.mqtt://test.mosquitto.org:1883", callbackCallback)

    -- The only message callback for both JSON and binary data
    local function messageCallback(payload, err)
      if err then
        print("Error:" .. tostring(err))
      end
      ua.printTable("payload", payload)
    end

    -- Subscribe on a topic with binary data
    mqttClient:subscribe("rtl/uadp/data/urn:arykovanov-note:opcua:server/group/dataset", messageCallback)
    -- Subscribe on a topic with JSON data
    mqttClient:subscribe("rtl/json/data/urn:arykovanov-note:opcua:server/group/dataset", messageCallback)

    -- Wait for some time
    local cnt=1
    while cnt <= 3 do
       ba.sleep(1000)
       cnt = cnt+1
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/pubsub/mqtt_subscribe.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="mqtt.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="MQTT PubSub API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="mqtt_message_format.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="UADP Message">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/pubsub/mqtt_message_format.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="../server/index.html" class="reference internal">Server</a>
- <a href="index.html" class="reference internal">Publish Subscribe API</a>
  - <a href="client_server_vs_pubsub.html" class="reference internal">Client/Server vs PubSub over MQTT</a>
  - <a href="mqtt.html" class="reference internal">MQTT PubSub API</a>
  - <a href="mqtt_examples.html" class="reference internal">MQTT PubSub Examples</a>
  - <a href="#" class="current reference internal">UADP Message</a>
    - <a href="#common-fields" class="reference internal">Common Fields</a>
      - <a href="#groupheader" class="reference internal">GroupHeader</a>
      - <a href="#securityheader" class="reference internal">SecurityHeader</a>
    - <a href="#uadp-datasetmessage" class="reference internal">UADP DataSetMessage</a>
    - <a href="#uadp-announcementmessage" class="reference internal">UADP AnnouncementMessage</a>
      - <a href="#dataset-metadata" class="reference internal">DATASET_METADATA</a>
      - <a href="#metadata" class="reference internal">Metadata</a>
      - <a href="#metadata-fields" class="reference internal">Metadata Fields</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Publish Subscribe API](index.html)
- UADP Message
- <a href="../_sources/pubsub/mqtt_message_format.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="uadp-message" class="section">

# UADP Message<a href="#uadp-message" class="headerlink" title="Link to this heading"></a>

This page summarizes the Lua table fields used for UADP PubSub messages.

<div id="common-fields" class="section">

## Common Fields<a href="#common-fields" class="headerlink" title="Link to this heading"></a>

| Field                                            | Type                       | Notes                                                                                                         |
|--------------------------------------------------|----------------------------|---------------------------------------------------------------------------------------------------------------|
| <span class="pre">`PublisherId`</span>           | number or string, optional | Publisher identifier.                                                                                         |
| <span class="pre">`PublisherIdType`</span>       | optional                   | Detected from <span class="pre">`PublisherId`</span> when omitted.                                            |
| <span class="pre">`DataSetClassId`</span>        | GUID, optional             | Dataset class identifier.                                                                                     |
| <span class="pre">`GroupHeader`</span>           | table, optional            | Writer group and sequence information.                                                                        |
| <span class="pre">`TimeStamp`</span>             | DateTime                   | Message timestamp.                                                                                            |
| <span class="pre">`Picoseconds`</span>           | UInt16                     | Additional timestamp precision.                                                                               |
| <span class="pre">`SecurityHeader`</span>        | table                      | Security flags and token information.                                                                         |
| <span class="pre">`DataSetMessages`</span>       | array                      | Dataset messages in the network message.                                                                      |
| <span class="pre">`DiscoveryAnnouncement`</span> | table                      | Discovery announcement. Used instead of <span class="pre">`DataSetMessages`</span> for announcement messages. |
| <span class="pre">`SecurityFooter`</span>        | table                      | Optional security footer.                                                                                     |
| <span class="pre">`Signature`</span>             | ByteString                 | Optional message signature.                                                                                   |

<div id="groupheader" class="section">

### GroupHeader<a href="#groupheader" class="headerlink" title="Link to this heading"></a>

| Field                                           | Type   | Notes                            |
|-------------------------------------------------|--------|----------------------------------|
| <span class="pre">`WriterGroupId`</span>        | UInt16 | Writer group identifier.         |
| <span class="pre">`GroupVersion`</span>         | UInt32 | Writer group version.            |
| <span class="pre">`NetworkMessageNumber`</span> | UInt16 | Network message number.          |
| <span class="pre">`SequenceNumber`</span>       | UInt16 | Network message sequence number. |

</div>

<div id="securityheader" class="section">

### SecurityHeader<a href="#securityheader" class="headerlink" title="Link to this heading"></a>

| Field                                                     | Type       | Notes                            |
|-----------------------------------------------------------|------------|----------------------------------|
| <span class="pre">`NetworkMessageSignedEnabled`</span>    | boolean    | Network message signature flag.  |
| <span class="pre">`NetworkMessageEncryptedEnabled`</span> | boolean    | Network message encryption flag. |
| <span class="pre">`SecurityFooterEnabled`</span>          | boolean    | Security footer flag.            |
| <span class="pre">`ForceKeyResetEnabled`</span>           | boolean    | Key reset flag.                  |
| <span class="pre">`SecurityTokenID`</span>                | UInt32     | Security token identifier.       |
| <span class="pre">`MessageNonce`</span>                   | ByteString | Message nonce.                   |
| <span class="pre">`SecurityFooterSize`</span>             | UInt16     | Security footer size.            |

</div>

</div>

<div id="uadp-datasetmessage" class="section">

## UADP DataSetMessage<a href="#uadp-datasetmessage" class="headerlink" title="Link to this heading"></a>

| Field                                                       | Type        | Notes                            |
|-------------------------------------------------------------|-------------|----------------------------------|
| <span class="pre">`DataSetWriterId`</span>                  | UInt16      | Dataset writer identifier.       |
| <span class="pre">`DataSetMessageSequenceNumber`</span>     | UInt16      | Dataset message sequence number. |
| <span class="pre">`Timestamp`</span>                        | DateTime    | Dataset message timestamp.       |
| <span class="pre">`PicoSeconds`</span>                      | UInt16      | Additional timestamp precision.  |
| <span class="pre">`Status`</span>                           | UInt16      | Dataset message status.          |
| <span class="pre">`ConfigurationVersionMajorVersion`</span> | UInt16      | Major configuration version.     |
| <span class="pre">`ConfigurationVersionMinorVersion`</span> | UInt16      | Minor configuration version.     |
| <span class="pre">`DataSetMessageType`</span>               | 4-bit value | Dataset message type.            |
| <span class="pre">`FieldEncoding`</span>                    | 2-bit value | Field encoding type.             |
| <span class="pre">`Fields`</span>                           | array       | Published fields.                |
| <span class="pre">`PagingSize`</span>                       | number      | Paging size.                     |
| <span class="pre">`Padding`</span>                          | ByteString  | Padding bytes.                   |

<span class="pre">`Fields`</span> entries use this shape:

| Field                            | Type                | Notes        |
|----------------------------------|---------------------|--------------|
| <span class="pre">`Index`</span> | number              | Field index. |
| <span class="pre">`Value`</span> | any supported value | Field value. |

</div>

<div id="uadp-announcementmessage" class="section">

## UADP AnnouncementMessage<a href="#uadp-announcementmessage" class="headerlink" title="Link to this heading"></a>

<div id="dataset-metadata" class="section">

### DATASET_METADATA<a href="#dataset-metadata" class="headerlink" title="Link to this heading"></a>

| Field                                                      | Type                                                              | Notes                         |
|------------------------------------------------------------|-------------------------------------------------------------------|-------------------------------|
| <span class="pre">`DiscoveryAnnouncement`</span>           | table                                                             | Root announcement object.     |
| <span class="pre">`AnnouncementType`</span>                | <span class="pre">`uadp.announcementType.DATASET_METADATA`</span> | Announcement kind.            |
| <span class="pre">`SequenceNumber`</span>                  | UInt16                                                            | Announcement sequence number. |
| <span class="pre">`DatasetMetadata`</span>                 | table                                                             | Dataset metadata payload.     |
| <span class="pre">`DatasetMetadata.DataSetWriterId`</span> | UInt16                                                            | Dataset writer identifier.    |
| <span class="pre">`DatasetMetadata.StatusCode`</span>      | StatusCode                                                        | Metadata status.              |
| <span class="pre">`DatasetMetadata.Metadata`</span>        | table                                                             | Metadata content.             |

</div>

<div id="metadata" class="section">

### Metadata<a href="#metadata" class="headerlink" title="Link to this heading"></a>

| Field                                           | Type            | Notes                                                                                         |
|-------------------------------------------------|-----------------|-----------------------------------------------------------------------------------------------|
| <span class="pre">`Namespaces`</span>           | string array    | Namespace URI list.                                                                           |
| <span class="pre">`StructureDataTypes`</span>   | array           | Structure data type metadata.                                                                 |
| <span class="pre">`EnumDataTypes`</span>        | array           | Enum data type metadata.                                                                      |
| <span class="pre">`SimpleDataTypes`</span>      | array           | Simple data type metadata.                                                                    |
| <span class="pre">`Name`</span>                 | string          | Dataset name.                                                                                 |
| <span class="pre">`Description`</span>          | LocalizedText   | Dataset description.                                                                          |
| <span class="pre">`DataSetClassId`</span>       | GUID            | Dataset class identifier.                                                                     |
| <span class="pre">`ConfigurationVersion`</span> | table           | Contains <span class="pre">`MajorVersion`</span> and <span class="pre">`MinorVersion`</span>. |
| <span class="pre">`Fields`</span>               | array of tables | Dataset field metadata.                                                                       |

</div>

<div id="metadata-fields" class="section">

### Metadata Fields<a href="#metadata-fields" class="headerlink" title="Link to this heading"></a>

| Field                                      | Type          | Notes                                                                                                     |
|--------------------------------------------|---------------|-----------------------------------------------------------------------------------------------------------|
| <span class="pre">`Name`</span>            | string        | Field name.                                                                                               |
| <span class="pre">`Description`</span>     | LocalizedText | Field description.                                                                                        |
| <span class="pre">`FieldFlags`</span>      | number        | Field flags.                                                                                              |
| <span class="pre">`BuiltInType`</span>     | number        | Built-in type identifier.                                                                                 |
| <span class="pre">`DataType`</span>        | NodeId        | Data type NodeId.                                                                                         |
| <span class="pre">`ValueRank`</span>       | Int32         | Value rank.                                                                                               |
| <span class="pre">`ArrayDimensions`</span> | array         | Array dimensions.                                                                                         |
| <span class="pre">`MaxStringLength`</span> | UInt32        | Maximum string length.                                                                                    |
| <span class="pre">`DataSetFieldId`</span>  | GUID          | Dataset field identifier.                                                                                 |
| <span class="pre">`Properties`</span>      | array         | Field properties. Each property has <span class="pre">`Name`</span> and <span class="pre">`Value`</span>. |

</div>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="mqtt_examples.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="MQTT PubSub Examples"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="../integrations/index.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Examples">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/secure_policy.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="about.html" class="reference internal">About opcua-lua</a>
- <a href="opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="model/index.html" class="reference internal">Address Space API</a>
- <a href="client/index.html" class="reference internal">Client</a>
- <a href="server/index.html" class="reference internal">Server</a>
- <a href="pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="integrations/index.html" class="reference internal">Examples</a>
- <a href="logging.html" class="reference internal">Logging subsystem</a>
- <a href="#" class="current reference internal">Secure Policy</a>
  - <a href="#none" class="reference internal">None</a>
  - <a href="#basic128rsa15" class="reference internal">Basic128Rsa15</a>
  - <a href="#aes128-sha256-rsaoaep" class="reference internal">Aes128_Sha256_RsaOaep</a>
  - <a href="#basic256sha256" class="reference internal">Basic256Sha256</a>
  - <a href="#establishing-secure-connection" class="reference internal">Establishing secure connection</a>
- <a href="application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="types.html" class="reference internal">Data Types</a>
- <a href="interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="index.html" class="icon icon-home" aria-label="Home"></a>
- Secure Policy
- <a href="_sources/secure_policy.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="secure-policy" class="section">

# Secure Policy<a href="#secure-policy" class="headerlink" title="Link to this heading"></a>

Security policy determines what encryption and signature algorithms are applied to messages. The configuration file defines the list of security policies that the server or client can use.

<div id="none" class="section">

## None<a href="#none" class="headerlink" title="Link to this heading"></a>

The secure policy None does not apply any protection to messages. Use this secure policy only for testing purposes. The following example shows how to configure a client with the ability to communicate with the server without encryption:

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local config = {
      applicationName = 'RealTimeLogic example',
      applicationUri = "urn:opcua-lua:example",
      productUri = "urn:opcua-lua:example",
      securePolicies = {
        { -- #1
          securityPolicyUri = ua.SecurityPolicy.None
        }
      }
    }

    local client = ua.newClient(config)
    trace("connecting to server")
    local endpointUrl = "opc.tcp://localhost:4841"
    local err = client:connect(endpointUrl)
    if err ~= nil then
      trace("connection failed: "..err)
    else
      trace("Connected sucessfully")
    end

    client:disconnect()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/client_connect.lua" class="reference external">Full source</a>

</div>

<div id="basic128rsa15" class="section">

## Basic128Rsa15<a href="#basic128rsa15" class="headerlink" title="Link to this heading"></a>

<div class="admonition warning">

Warning

This policy is considered deprecated and should be used only for compatibility with old systems.

</div>

Secure policy Basic128Rsa15 applies the following algorithms to messages:

- Asymmetric encryption RSAES-PKCS1-v1_5 with key size 1024 or 2048 bits.

- Asymmetric signature algorithm RSASSA-PKCS1-v1_5 with SHA-1 hash function

- Symmetric encryption AES-128-CBC

The following example shows how to configure Basic128Rsa15 secure policy:

<div class="highlight-lua notranslate">

<div class="highlight">

    local clientConfig = {
      applicationName = 'RealTimeLogic example',
      applicationUri = "urn:localhost:RealTimeLogic",

      io = _G.io,
      certificate = mako.cfgdir.."/../certs/client.pem",
      key =         mako.cfgdir.."/../certs/client.key",

      securePolicies = {
        { -- #1 Required to discover secure policies
          securityPolicyUri = ua.SecurityPolicy.None
        },
        { -- #2
          securityPolicyUri = ua.SecurityPolicy.Basic128Rsa15,
          securityMode = ua.MessageSecurityMode.SignAndEncrypt,
        }
      },
    }

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/policy_basic128rsa15.lua" class="reference external">Full source</a>

</div>

<div id="aes128-sha256-rsaoaep" class="section">

## Aes128_Sha256_RsaOaep<a href="#aes128-sha256-rsaoaep" class="headerlink" title="Link to this heading"></a>

Secure policy Aes128_Sha256_RsaOaep applies the following algorithms to messages:

- Asymmetric encryption RSAES-OAEP with SHA-1 hash function.

- Asymmetric signature algorithm RSASSA-PKCS1-v1_5 with SHA2-256 hash function

- Symmetric encryption AES-128-CBC

The following example shows how to configure Aes128_Sha256_RsaOaep secure policy:

<div class="highlight-lua notranslate">

<div class="highlight">

    local clientConfig = {
      applicationName = 'RealTimeLogic example',
      applicationUri = "urn:localhost:RealTimeLogic",

      io = _G.io,
      certificate = mako.cfgdir.."/../certs/client.pem",
      key =         mako.cfgdir.."/../certs/client.key",

      securePolicies = {
        { -- #1 Required to discover secure policies
          securityPolicyUri = ua.SecurityPolicy.None
        },
        { -- #2
          securityPolicyUri = ua.SecurityPolicy.Aes128_Sha256_RsaOaep,
          securityMode = ua.MessageSecurityMode.SignAndEncrypt,
        }
      },
    }

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/policy_aes128_sha256_rsaoaep.lua" class="reference external">Full source</a>

</div>

<div id="basic256sha256" class="section">

## Basic256Sha256<a href="#basic256sha256" class="headerlink" title="Link to this heading"></a>

Secure policy Basic256Sha256 applies the following algorithms to messages:

- Asymmetric encryption RSAES-OAEP with SHA-1 hash function.

- Asymmetric signature algorithm RSASSA-PKCS1-v1_5 with SHA2-256 hash function

- Symmetric encryption AES-128-CBC

The following example shows how to configure Basic256Sha256 secure policy:

<div class="highlight-lua notranslate">

<div class="highlight">

    local clientConfig = {
      applicationName = 'RealTimeLogic example',
      applicationUri = "urn:localhost:RealTimeLogic",

      io = _G.io,
      certificate = mako.cfgdir.."/../certs/client.pem",
      key =         mako.cfgdir.."/../certs/client.key",

      securePolicies = {
        { -- #1 Required to discover secure policies
          securityPolicyUri = ua.SecurityPolicy.None
        },
        { -- #2
          securityPolicyUri = ua.SecurityPolicy.Basic256Sha256,
          securityMode = ua.MessageSecurityMode.SignAndEncrypt,
        }
      },
    }

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/policy_basic256sha256.lua" class="reference external">Full source</a>

</div>

<div id="establishing-secure-connection" class="section">

## Establishing secure connection<a href="#establishing-secure-connection" class="headerlink" title="Link to this heading"></a>

When a client connects to a server, it must open a secure channel and specify a secure policy. To discover the exact secure policy parameters, the client can use a GetEndpoints request. In the GetEndpoints response, the server will return all known endpoints with applicable secure policies. The following example shows how to discover an endpoint with the secure policy Basic128Rsa15:

<div class="highlight-lua notranslate">

<div class="highlight">

    local c = ua.newClient(clientConfig)

    -- Connecto to server
    err = c:connect("opc.tcp://localhost:4841")
    if err ~= nil then error(err) end

    -- Open channel with secure policy None. In this mode server
    -- should allow to call endpoint services
    resp, err = c:openSecureChannel(3600000,
      ua.SecurityPolicy.None, ua.MessageSecurityMode.None)

    -- Select endpoints
    resp, err = c:getEndpoints()
    if err ~= nil then error(err) end

    -- close secure channel (TCP connection is still alive)
    err = c:closeSecureChannel()

    -- Search secure policy
    local basic128rsa15
    for _, endpoint in ipairs(resp.Endpoints) do
      if endpoint.SecurityPolicyUri == ua.SecurityPolicy.Basic128Rsa15 then
        basic128rsa15 = endpoint
        break
      end
    end

    if not basic128rsa15 then
      error("Cannot find Basic128Rsa15 policy on the server")
    end

    -- Open secure channel. Specify secure policy and server certificate
    resp, err = c:openSecureChannel(
      3600000,
      ua.SecurityPolicy.Basic128Rsa15, ua.MessageSecurityMode.SignAndEncrypt,
      basic128rsa15.ServerCertificate)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/client/policy_basic128rsa15.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="logging.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Logging subsystem"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="application_certificate.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Application Certificate">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/server/authentication.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="index.html" class="reference internal">Server</a>
  - <a href="getting_started.html" class="reference internal">Getting Started with Server</a>
  - <a href="server_config.html" class="reference internal">Configuration table</a>
  - <a href="data_source.html" class="reference internal">Exporting Device Data</a>
  - <a href="xml_models.html" class="reference internal">XML Models</a>
  - <a href="#" class="current reference internal">Server side authentication</a>
    - <a href="#identity-token-policies" class="reference internal">Identity token policies</a>
    - <a href="#authentication-callback" class="reference internal">Authentication callback</a>
    - <a href="#anonymous-token" class="reference internal">Anonymous token</a>
    - <a href="#user-name-and-password" class="reference internal">User Name and password</a>
    - <a href="#x509-user-certificate" class="reference internal">X509 user certificate</a>
    - <a href="#issued-tokens" class="reference internal">Issued tokens</a>
  - <a href="init_opcua_project.html" class="reference internal">OPC UA Server Project Code Generator</a>
  - <a href="http.html" class="reference internal">HTTP Server</a>
  - <a href="server_api.html" class="reference internal">Server API</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Server](index.html)
- Server side authentication
- <a href="../_sources/server/authentication.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="server-side-authentication" class="section">

# Server side authentication<a href="#server-side-authentication" class="headerlink" title="Link to this heading"></a>

Almost all OPC UA requests require a session. A session is created and bound to a secure channel. After being created, the session must be activated. After being activated, the client provides user credentials to the server. Credentials are represented in the form of identity tokens.

The OPC UA stack supports authentication using the following identity token types:

> <div>
>
> - Anonymous
>
> - User name and password
>
> - X509 certificate
>
> - JWT token
>
> - OAuth2
>
> - Azure
>
> </div>

The OPC UA client and server are responsible for delivering authentication tokens, while the generation and validation of tokens should be implemented separately.

One of the tasks of the OPC UA server stack is to manage sessions, including their timeouts and the processing of user tokens. The server tracks the lifetime of sessions and checks if requests are being performed under a valid session.

During session activation, the server receives a user token, decrypts it, and checks its validity. The user represented by the token is not validated, leaving the validation process up to the developer.

<div id="identity-token-policies" class="section">

## Identity token policies<a href="#identity-token-policies" class="headerlink" title="Link to this heading"></a>

To support user authentication, it is necessary to configure user identity token policies. The server returns to the client the parameters of supported identity token policies. Using this information, the client can create a user token with the required parameters.

The user identity policies on the server are specified in the *userIdentityTokens* field:

<div class="highlight-lua notranslate">

<div class="highlight">

    local function authenticate(tokenType, token, user)

      if tokenType == "anonymous" then
        return true
      elseif tokenType == "username" then
        return user == "admin" and token == "12345"
      elseif tokenType == "x509" then
        local cert = ba.parsecert(token)
        if cert.subject.commonname ~= "admin" then
          return false
        end

        return true
      end

      return false
    end


    local config = {
      endpoints = {
        {
          endpointUrl = "opc.tcp://localhost:4845",
        }
      },

      io = _G.io,
      certificate = mako.cfgdir.."/../certs/client.pem",
      key =         mako.cfgdir.."/../certs/client.key",

      securePolicies = {
        { -- #1
          securityPolicyUri = ua.SecurityPolicy.None,
        },
        { -- #2
          securityPolicyUri = ua.SecurityPolicy.Basic128Rsa15,
          securityMode = ua.MessageSecurityMode.SignAndEncrypt,
        }
      },

      -- Parameters of user tokens
      userIdentityTokens = {
        {
          policyId = "anonymous",
          tokenType = ua.UserTokenType.Anonymous
        },
        {
          policyId = "username",
          tokenType = ua.UserTokenType.UserName,
          securityPolicyUri = ua.SecurityPolicy.None
        },
        {
          policyId = "username_basic128Rsa15",

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_configure_auth.lua" class="reference external">Full source</a>

</div>

<div id="authentication-callback" class="section">

## Authentication callback<a href="#authentication-callback" class="headerlink" title="Link to this heading"></a>

When the server receives a user token from the client, it first validates its consistency and checks its security parameters. Then, it is required to check the content of the token, such as the username and password, certificate validity, JWT signatures, roles, etc. To perform all of these checks, the server can call an authentication callback function. This callback is specified in the server configuration file in the authenticate field.

The following example demonstrates an implementation of such a function:

<div class="highlight-lua notranslate">

<div class="highlight">

      if tokenType == "anonymous" then
        return true
      elseif tokenType == "username" then
        return user == "admin" and token == "12345"
      elseif tokenType == "x509" then
        local cert = ba.parsecert(token)
        if cert.subject.commonname ~= "admin" then
          return false
        end

        return true
      end

      return false
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_configure_auth.lua" class="reference external">Full source</a>

The authentication callback function receives a token, validates the token, and must return a boolean value:

- *true* - indicates that the server allows access

- *false* - indicates that the server rejects access.

The authentication callback is called with specific parameters for each type of token. The following sections describe the parameters for each supported token type.

</div>

<div id="anonymous-token" class="section">

## Anonymous token<a href="#anonymous-token" class="headerlink" title="Link to this heading"></a>

To use anonymous (or absent) authentication, you need to add the following element to the userIdentityTokens configuration table:

<div class="highlight-lua notranslate">

<div class="highlight">

    local userIdentityTokens = {
      {
        policyId = "anonymous",
        tokenType = ua.UserTokenType.Anonymous
      }
    }

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_auth_anonymous.lua" class="reference external">Full source</a>

The authentication callback is called with only the tokenType equal to “anonymous”. The example:

<div class="highlight-lua notranslate">

<div class="highlight">

    local function authenticate(tokenType)

      if tokenType == "anonymous" then
        return true
      end

      return false
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_auth_anonymous.lua" class="reference external">Full source</a>

</div>

<div id="user-name-and-password" class="section">

## User Name and password<a href="#user-name-and-password" class="headerlink" title="Link to this heading"></a>

Configuration file should contain a section with username token policy:

<div class="highlight-lua notranslate">

<div class="highlight">

    local userIdentityTokens = {
      {
        policyId = "username",
        tokenType = ua.UserTokenType.UserName,
        securityPolicyUri = ua.SecurityPolicy.None
      },
      {
        policyId = "username_basic128Rsa15",
        tokenType = ua.UserTokenType.UserName,
        securityPolicyUri = ua.SecurityPolicy.Basic128Rsa15
      },
    }

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_auth_username.lua" class="reference external">Full source</a>

If a token policy has a security policy, then the corresponding security policy must be configured in the securityPolicies field, as it contains the server certificate and key that are used for encrypting passwords. It’s important to configure the security policies correctly in order to ensure secure user authentication.

The authentication callback is called with these parameters:

| Parameter                            | Description                            |
|--------------------------------------|----------------------------------------|
| <span class="pre">`tokenType`</span> | <span class="pre">`"username"`</span>. |
| <span class="pre">`token`</span>     | Password.                              |
| <span class="pre">`userName`</span>  | User name.                             |

<div class="highlight-lua notranslate">

<div class="highlight">

    local function authenticate(tokenType, token, user)

      if tokenType == "username" then
        return user == "admin" and token == "12345"
      end

      return false
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_auth_username.lua" class="reference external">Full source</a>

</div>

<div id="x509-user-certificate" class="section">

## X509 user certificate<a href="#x509-user-certificate" class="headerlink" title="Link to this heading"></a>

To enable certificate-based user authentication, the configuration file should include a section with the Certificate user identity policy. This allows clients to provide their certificate to the server for authentication:

<div class="highlight-lua notranslate">

<div class="highlight">

    local userIdentityTokens = {
      {
        policyId = "x509",
        tokenType = ua.UserTokenType.Certificate,
      },
    }

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_auth_x509.lua" class="reference external">Full source</a>

The authentication callback is called with these parameters:

| Parameter                            | Description                        |
|--------------------------------------|------------------------------------|
| <span class="pre">`tokenType`</span> | <span class="pre">`"x509"`</span>. |
| <span class="pre">`token`</span>     | Certificate in DER format.         |

<div class="highlight-lua notranslate">

<div class="highlight">

    local function authenticate(tokenType, token)

      if tokenType == "x509" then
        local cert = ba.parsecert(token)
        return cert.subject.commonname == "admin"
      end

      return false
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_auth_x509.lua" class="reference external">Full source</a>

</div>

<div id="issued-tokens" class="section">

## Issued tokens<a href="#issued-tokens" class="headerlink" title="Link to this heading"></a>

The OPC UA server can support authentication using tokens issued by third-party identity services. These types of tokens can be useful in allowing users to authenticate using their existing login credentials from other systems or services, without the need to create new usernames and passwords for the OPC UA server:

The OPC UA server currently supports the following types of issued tokens:

- *JWT*

- *Azure*

- *OAuth2*

To configure authentication using issued tokens, it is necessary to add a section with tokenType equal to IssuedToken. Each section should include these fields:

| Field                                        | Description                                                                                        |
|----------------------------------------------|----------------------------------------------------------------------------------------------------|
| <span class="pre">`policyId`</span>          | Policy name.                                                                                       |
| <span class="pre">`tokenType`</span>         | <span class="pre">`IssuedToken`</span>.                                                            |
| <span class="pre">`issuerEndpointUrl`</span> | URL of the identity server that issues tokens.                                                     |
| <span class="pre">`issuedTokenType`</span>   | Issued token type, such as JWT, Azure, or OAuth2.                                                  |
| <span class="pre">`securityPolicyUri`</span> | Encryption parameters for the token. If absent, the token is sent without token-policy encryption. |

Here is an example of how to configure different issued token policies:

<div class="highlight-lua notranslate">

<div class="highlight">

    local issuedIdentityTokens = {
      {
        policyId = "jwt",
        tokenType = ua.UserTokenType.IssuedToken,
        issuedTokenType = ua.IssuedTokenType.JWT,
        securityPolicyUri = ua.SecurityPolicy.Basic128Rsa15
      },
      {
        policyId = "jwt",
        tokenType = ua.UserTokenType.IssuedToken,
        issuedTokenType = ua.IssuedTokenType.JWT,
        securityPolicyUri = ua.SecurityPolicy.Basic128Rsa15
      },
      {
        policyId = "azure",
        tokenType = ua.UserTokenType.IssuedToken,
        issuedTokenType = ua.IssuedTokenType.Azure,
        securityPolicyUri = ua.SecurityPolicy.Basic128Rsa15
      },
      {
        policyId = "azure",
        tokenType = ua.UserTokenType.IssuedToken,
        issuedTokenType = ua.IssuedTokenType.Azure,
      },
      {
        policyId = "oauth2",
        tokenType = ua.UserTokenType.IssuedToken,
        issuedTokenType = ua.IssuedTokenType.OAuth2,
        securityPolicyUri = ua.SecurityPolicy.Basic128Rsa15
      },
    }

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_auth_issued_token.lua" class="reference external">Full source</a>

The authentication callback for all types of issued tokens is called with these parameters:

| Parameter                                    | Description                                                                                                                               |
|----------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------|
| <span class="pre">`tokenType`</span>         | <span class="pre">`"jwt"`</span>, <span class="pre">`"oauth2"`</span>, or <span class="pre">`"azure"`</span>.                             |
| <span class="pre">`token`</span>             | Token content.                                                                                                                            |
| <span class="pre">`issuerEndpointUrl`</span> | URL of the server that issued the token. The value comes from the matching token policy in <span class="pre">`userIdentityTokens`</span>. |

<div class="highlight-lua notranslate">

<div class="highlight">

    local function authenticate(tokenType, token, issuerEnpointUrl)

      if tokenType == "jwt" then
        return true
      elseif tokenType == "azure" then
        return true
      elseif tokenType == "oauth2" then
        return true
      end

      return false
    end

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_auth_issued_token.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="xml_models.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="XML Models"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="init_opcua_project.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="OPC UA Server Project Code Generator">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/server/data_source.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="index.html" class="reference internal">Server</a>
  - <a href="getting_started.html" class="reference internal">Getting Started with Server</a>
  - <a href="server_config.html" class="reference internal">Configuration table</a>
  - <a href="#" class="current reference internal">Exporting Device Data</a>
    - <a href="#value-callback" class="reference internal">Value Callback</a>
      - <a href="#callback" class="reference internal"><span class="pre"><code class="docutils literal notranslate">callback()</code></span></a>
    - <a href="#writing-callback-parameters" class="reference internal">Writing Callback Parameters</a>
    - <a href="#reading-callback-parameters" class="reference internal">Reading Callback Parameters</a>
    - <a href="#error-reporting" class="reference internal">Error reporting</a>
    - <a href="#custom-data-source-example" class="reference internal">Custom data source example</a>
  - <a href="xml_models.html" class="reference internal">XML Models</a>
  - <a href="authentication.html" class="reference internal">Server side authentication</a>
  - <a href="init_opcua_project.html" class="reference internal">OPC UA Server Project Code Generator</a>
  - <a href="http.html" class="reference internal">HTTP Server</a>
  - <a href="server_api.html" class="reference internal">Server API</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Server](index.html)
- Exporting Device Data
- <a href="../_sources/server/data_source.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="exporting-device-data" class="section">


# Exporting Device Data<a href="#exporting-device-data" class="headerlink" title="Link to this heading"></a>

The main purpose of an OPC UA server is to provide access to real world objects. External information is stored in the address space in the form of variable nodes. Variable nodes have value attributes. The following section shows how to connect attributes with real world objects.

<div id="value-callback" class="section">

## Value Callback<a href="#value-callback" class="headerlink" title="Link to this heading"></a>

The value callback is a function and is used for both reading and writing.

<span class="sig-name descname"><span class="pre">callback</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">nodeId</span></span>*<span class="optional">\[</span>, *<span class="n"><span class="pre">value</span></span>*<span class="optional">\]</span><span class="sig-paren">)</span><a href="#callback" class="headerlink" title="Link to this definition"></a>  

</div>

<div id="writing-callback-parameters" class="section">

## Writing Callback Parameters<a href="#writing-callback-parameters" class="headerlink" title="Link to this heading"></a>

nodeId<span class="colon">:</span>  
of type <a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> The Node ID of the node to modify.

value<span class="colon">:</span>  
of type <a href="../types.html#data-value-type" class="reference internal"><span class="std std-ref">DataValue</span></a> For write: The new variable value set by the client.

return<span class="colon">:</span>  
ignored

</div>

<div id="reading-callback-parameters" class="section">

## Reading Callback Parameters<a href="#reading-callback-parameters" class="headerlink" title="Link to this heading"></a>

nodeId<span class="colon">:</span>  
of type <a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> The Node ID of the node to read.

return<span class="colon">:</span>  
<a href="../types.html#data-value-type" class="reference internal"><span class="std std-ref">DataValue</span></a> Data from device for read operation. Ignored for write operation.

</div>

<div id="error-reporting" class="section">

## Error reporting<a href="#error-reporting" class="headerlink" title="Link to this heading"></a>

To report an error, the callback should raise a Lua error with a suitable OPC UA status code.

</div>

<div id="custom-data-source-example" class="section">

## Custom data source example<a href="#custom-data-source-example" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Callback that will be called for Read/Write operations
    function callback(nodeId, newValue)
      if newValue ~= nil then
        -- writing data
        ua.printTable("newValue", newValue, trace)
        ua.printTable("deviceData", deviceData, trace)

        if newValue == nil then
          error(ua.StatusCode.BadInvalidArgument)
        end

        deviceData = newValue
      else
        -- reading data
        ua.printTable("read deviceData", deviceData, trace)
        return deviceData
      end
    end

    server:setValueCallback(dataNodeId, callback)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_value_callback.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="server_config.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Configuration table"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="xml_models.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="XML Models">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/server/getting_started.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="index.html" class="reference internal">Server</a>
  - <a href="#" class="current reference internal">Getting Started with Server</a>
    - <a href="#creating-a-server" class="reference internal">Creating a Server</a>
    - <a href="#adding-nodes-on-server" class="reference internal">Adding Nodes on server</a>
    - <a href="#starting-the-server" class="reference internal">Starting the Server</a>
    - <a href="#stopping-the-server" class="reference internal">Stopping the Server</a>
  - <a href="server_config.html" class="reference internal">Configuration table</a>
  - <a href="data_source.html" class="reference internal">Exporting Device Data</a>
  - <a href="xml_models.html" class="reference internal">XML Models</a>
  - <a href="authentication.html" class="reference internal">Server side authentication</a>
  - <a href="init_opcua_project.html" class="reference internal">OPC UA Server Project Code Generator</a>
  - <a href="http.html" class="reference internal">HTTP Server</a>
  - <a href="server_api.html" class="reference internal">Server API</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Server](index.html)
- Getting Started with Server
- <a href="../_sources/server/getting_started.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="getting-started-with-server" class="section">

# Getting Started with Server<a href="#getting-started-with-server" class="headerlink" title="Link to this heading"></a>

<div id="creating-a-server" class="section">

## Creating a Server<a href="#creating-a-server" class="headerlink" title="Link to this heading"></a>

The OPC UA stack is provided as a Lua module and must be loaded as follows:

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_getting_started.lua" class="reference external">Full source</a>

The returned value ‘ua’ is a table with functions that may be used to create an OPC UA server and client.

To create a server you need to call ‘ua.newServer’ function. The Lua function “newServer” returns an OPC UA server instance. Before it can be started, the server instance must be initialized.

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Create new OPC UA server instance.
    -- Pass configuration table to server.
    local server = ua.newServer()

    -- Initialize server.
    server:initialize()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_getting_started.lua" class="reference external">Full source</a>

It is possible to pass a Lua table with configuration parameters into function ‘newServer’. See the section <a href="server_config.html#config-table" class="reference internal"><span class="std std-ref">Configuration table</span></a> for additional details

<div class="highlight-lua notranslate">

<div class="highlight">

    local config = {
      endpoints = {
        {
          endpointUrl = "opc.tcp://localhost:4845",
        }
      },

      securePolicies = {
        { -- #1
          securityPolicyUri = ua.SecurityPolicy.None,
        },
      },
    }

    local server = ua.newServer(config)
    server:initialize()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_auth_anonymous.lua" class="reference external">Full source</a>

</div>

<div id="adding-nodes-on-server" class="section">

## Adding Nodes on server<a href="#adding-nodes-on-server" class="headerlink" title="Link to this heading"></a>

Once the server is initialized, you can customize the server by adding new nodes.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ObjectsFolder = "i=85"

    -- Add two variables:
    --   1. Boolean scalar value
    --   2. Boolean array value

    -- required Node ID for variable
    local scalarBooleanId = "i=1000000"

    -- Initial boolean scalar value
    local scalarBoolean = {
      Type = ua.VariantType.Boolean,
      Value = true
    }

    local arrBooleanId = "i=1000001"
    -- Initial boolean array value
    local arrBoolean = {
      Type = ua.VariantType.Boolean,
      IsArray = true,
      Value = {true, false, true, false}
    }

    local model = server.model:edit()
    local objects = model:objectsFolder()
    local folder = objects:addFolder("NewFolder")
    folder:addVariable("Boolean", scalarBoolean, nil, scalarBooleanId)
    folder:addVariable("BooleanArray", arrBoolean, nil, arrBooleanId)
    model:save()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_getting_started.lua" class="reference external">Full source</a>

Managing nodes is described in <a href="../model/index.html#address-space-api" class="reference internal"><span class="std std-ref">Address Space API</span></a> section.

</div>

<div id="starting-the-server" class="section">

## Starting the Server<a href="#starting-the-server" class="headerlink" title="Link to this heading"></a>

After calling the ‘run’ method, you may connect to the OPC UA server instance using any OPC UA client. See the <a href="../integrations/thirdparty_clients.html#clients-to-server-tutorial" class="reference internal"><span class="std std-ref">How to Connect Third-Party Clients</span></a> for details.

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Start listening and dispatch incomming messages.
    server:run()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_getting_started.lua" class="reference external">Full source</a>

</div>

<div id="stopping-the-server" class="section">

## Stopping the Server<a href="#stopping-the-server" class="headerlink" title="Link to this heading"></a>

The server can be stopped, and the server’s listening socket can be closed by calling the ‘shutdown’ method.

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Stop server.
    server:shutdown()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_getting_started.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="index.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Server"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="server_config.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Configuration table">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/server/http.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="index.html" class="reference internal">Server</a>
  - <a href="getting_started.html" class="reference internal">Getting Started with Server</a>
  - <a href="server_config.html" class="reference internal">Configuration table</a>
  - <a href="data_source.html" class="reference internal">Exporting Device Data</a>
  - <a href="xml_models.html" class="reference internal">XML Models</a>
  - <a href="authentication.html" class="reference internal">Server side authentication</a>
  - <a href="init_opcua_project.html" class="reference internal">OPC UA Server Project Code Generator</a>
  - <a href="#" class="current reference internal">HTTP Server</a>
    - <a href="#example" class="reference internal">Example</a>
  - <a href="server_api.html" class="reference internal">Server API</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Server](index.html)
- HTTP Server
- <a href="../_sources/server/http.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="http-server" class="section">

# HTTP Server<a href="#http-server" class="headerlink" title="Link to this heading"></a>

To enable HTTP transport, you need to configure the server and add the corresponding endpoint URL. You don’t need to specify JSON or Binary encoding; the server will automatically detect it using the MIME type of HTTP request.

Currently, the server supports only the secure policy “None” for OPC UA messages over HTTP. Security in this case is provided by HTTPS. If you choose to use HTTP, be aware that messages are not encrypted or signed.

<div id="example" class="section">

## Example<a href="#example" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")

    local config = {
      endpoints = {
        {
          endpointUrl = "opc.http://localhost:9357/opcua",
        },
        {
          endpointUrl = "opc.https://localhost:9357/opcua",
        },
        {
          endpointUrl = "http://localhost:9357/opcua",
        },
        {
          endpointUrl = "https://localhost:9357/opcua",
        },
      },

      securePolicies = {
        { -- #1
          securityPolicyUri = ua.SecurityPolicy.None,
        },
      }
    }

    local server = ua.newServer(config)
    server:initialize()
    server:run()

    local onRequest = server:createHttpDirectory()

    --[[
    The onRequest function shoulbe called at LSP page when a HTTP request is received.
    The function is called with two arguments, the request and the response.

    <?lsp

       server:onRequest(request, response)

    ?>
    ]]

    server:shutdown()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_http.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="init_opcua_project.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="OPC UA Server Project Code Generator"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="server_api.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Server API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/server/index.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="#" class="current reference internal">Server</a>
  - <a href="getting_started.html" class="reference internal">Getting Started with Server</a>
  - <a href="server_config.html" class="reference internal">Configuration table</a>
  - <a href="data_source.html" class="reference internal">Exporting Device Data</a>
  - <a href="xml_models.html" class="reference internal">XML Models</a>
  - <a href="authentication.html" class="reference internal">Server side authentication</a>
  - <a href="init_opcua_project.html" class="reference internal">OPC UA Server Project Code Generator</a>
  - <a href="http.html" class="reference internal">HTTP Server</a>
  - <a href="server_api.html" class="reference internal">Server API</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Server
- <a href="../_sources/server/index.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="server" class="section">

# Server<a href="#server" class="headerlink" title="Link to this heading"></a>

<div class="toctree-wrapper compound">

- <a href="getting_started.html" class="reference internal">Getting Started with Server</a>
  - <a href="getting_started.html#creating-a-server" class="reference internal">Creating a Server</a>
  - <a href="getting_started.html#adding-nodes-on-server" class="reference internal">Adding Nodes on server</a>
  - <a href="getting_started.html#starting-the-server" class="reference internal">Starting the Server</a>
  - <a href="getting_started.html#stopping-the-server" class="reference internal">Stopping the Server</a>
- <a href="server_config.html" class="reference internal">Configuration table</a>
- <a href="data_source.html" class="reference internal">Exporting Device Data</a>
  - <a href="data_source.html#value-callback" class="reference internal">Value Callback</a>
    - <a href="data_source.html#callback" class="reference internal"><span class="pre"><code class="docutils literal notranslate">callback()</code></span></a>
  - <a href="data_source.html#writing-callback-parameters" class="reference internal">Writing Callback Parameters</a>
  - <a href="data_source.html#reading-callback-parameters" class="reference internal">Reading Callback Parameters</a>
  - <a href="data_source.html#error-reporting" class="reference internal">Error reporting</a>
  - <a href="data_source.html#custom-data-source-example" class="reference internal">Custom data source example</a>
- <a href="xml_models.html" class="reference internal">XML Models</a>
  - <a href="xml_models.html#loading-xml-models" class="reference internal">Loading XML models</a>
  - <a href="xml_models.html#export-xml-models" class="reference internal">Export XML models</a>
- <a href="authentication.html" class="reference internal">Server side authentication</a>
  - <a href="authentication.html#identity-token-policies" class="reference internal">Identity token policies</a>
  - <a href="authentication.html#authentication-callback" class="reference internal">Authentication callback</a>
  - <a href="authentication.html#anonymous-token" class="reference internal">Anonymous token</a>
  - <a href="authentication.html#user-name-and-password" class="reference internal">User Name and password</a>
  - <a href="authentication.html#x509-user-certificate" class="reference internal">X509 user certificate</a>
  - <a href="authentication.html#issued-tokens" class="reference internal">Issued tokens</a>
- <a href="init_opcua_project.html" class="reference internal">OPC UA Server Project Code Generator</a>
  - <a href="init_opcua_project.html#initializing-a-server" class="reference internal">Initializing a server</a>
- <a href="http.html" class="reference internal">HTTP Server</a>
  - <a href="http.html#example" class="reference internal">Example</a>
- <a href="server_api.html" class="reference internal">Server API</a>
  - <a href="server_api.html#server-constructor" class="reference internal">Server Constructor</a>
    - <a href="server_api.html#ua.newServer" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newServer()</code></span></a>
    - <a href="server_api.html#server-default-configuration" class="reference internal">Server default configuration</a>
    - <a href="server_api.html#server-custom-configuration" class="reference internal">Server custom configuration</a>
  - <a href="server_api.html#server-initialize" class="reference internal">server:initialize</a>
  - <a href="server_api.html#server-run" class="reference internal">server:run</a>
  - <a href="server_api.html#server-shutdown" class="reference internal">server:shutdown</a>
  - <a href="server_api.html#server-addnodes" class="reference internal">server:addNodes</a>
  - <a href="server_api.html#server-browse" class="reference internal">server:browse</a>
  - <a href="server_api.html#server-read" class="reference internal">server:read</a>
  - <a href="server_api.html#server-write" class="reference internal">server:write</a>
  - <a href="server_api.html#server-setvaluecallback" class="reference internal">server:setValueCallback</a>
  - <a href="server_api.html#server-setwritehook" class="reference internal">server:setWriteHook</a>
    - <a href="server_api.html#writeHook" class="reference internal"><span class="pre"><code class="docutils literal notranslate">writeHook()</code></span></a>
  - <a href="server_api.html#server-loadxmlmodels" class="reference internal">server:loadXmlModels</a>
  - <a href="server_api.html#server-createnamespace" class="reference internal">server:createNamespace</a>
  - <a href="server_api.html#server-exportxmlmodels" class="reference internal">server:exportXmlModels</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="../client/client_api.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Client API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="getting_started.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Getting Started with Server">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/server/init_opcua_project.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="index.html" class="reference internal">Server</a>
  - <a href="getting_started.html" class="reference internal">Getting Started with Server</a>
  - <a href="server_config.html" class="reference internal">Configuration table</a>
  - <a href="data_source.html" class="reference internal">Exporting Device Data</a>
  - <a href="xml_models.html" class="reference internal">XML Models</a>
  - <a href="authentication.html" class="reference internal">Server side authentication</a>
  - <a href="#" class="current reference internal">OPC UA Server Project Code Generator</a>
    - <a href="#initializing-a-server" class="reference internal">Initializing a server</a>
  - <a href="http.html" class="reference internal">HTTP Server</a>
  - <a href="server_api.html" class="reference internal">Server API</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Server](index.html)
- OPC UA Server Project Code Generator
- <a href="../_sources/server/init_opcua_project.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="opc-ua-server-project-code-generator" class="section">

# OPC UA Server Project Code Generator<a href="#opc-ua-server-project-code-generator" class="headerlink" title="Link to this heading"></a>

To help you with the initial configuration of your OPC UA project, you can use a script that will perform the initialization of a sample application with all required files. Initial configuration includes:

- Full configuration file

- Generate self-signed certificates for all secure policies

- Lua script for starting OPC UA server

<div id="initializing-a-server" class="section">

## Initializing a server<a href="#initializing-a-server" class="headerlink" title="Link to this heading"></a>

Create a Lua file with the following content:

<div class="highlight-lua notranslate">

<div class="highlight">

    local hostname = 'localhost'
    local applicationName = 'RealTimeLogic OPCUA Server'
    local applicationUri = 'urn:realtimelogic.com:opcua:server'

    local initServer = require('opcua.init').initializeServer
    initServer(hostname, applicationName, applicationUri)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/init_server.lua" class="reference external">Full source</a>

After running the script, you will see the following files:  
- config.lua - configuration file for OPC UA server

- basic128rsa15.key - private key for security policy Basic128rsa15

- basic128rsa15.pem - self-signed certificate for security policy Basic128rsa15

- main.lua - Lua script that runs OPC UA server

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="authentication.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Server side authentication"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="http.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="HTTP Server">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/server/server_api.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="index.html" class="reference internal">Server</a>
  - <a href="getting_started.html" class="reference internal">Getting Started with Server</a>
  - <a href="server_config.html" class="reference internal">Configuration table</a>
  - <a href="data_source.html" class="reference internal">Exporting Device Data</a>
  - <a href="xml_models.html" class="reference internal">XML Models</a>
  - <a href="authentication.html" class="reference internal">Server side authentication</a>
  - <a href="init_opcua_project.html" class="reference internal">OPC UA Server Project Code Generator</a>
  - <a href="http.html" class="reference internal">HTTP Server</a>
  - <a href="#" class="current reference internal">Server API</a>
    - <a href="#server-constructor" class="reference internal">Server Constructor</a>
      - <a href="#ua.newServer" class="reference internal"><span class="pre"><code class="docutils literal notranslate">ua.newServer()</code></span></a>
      - <a href="#server-default-configuration" class="reference internal">Server default configuration</a>
      - <a href="#server-custom-configuration" class="reference internal">Server custom configuration</a>
    - <a href="#server-initialize" class="reference internal">server:initialize</a>
    - <a href="#server-run" class="reference internal">server:run</a>
    - <a href="#server-shutdown" class="reference internal">server:shutdown</a>
    - <a href="#server-addnodes" class="reference internal">server:addNodes</a>
    - <a href="#server-browse" class="reference internal">server:browse</a>
    - <a href="#server-read" class="reference internal">server:read</a>
    - <a href="#server-write" class="reference internal">server:write</a>
    - <a href="#server-setvaluecallback" class="reference internal">server:setValueCallback</a>
    - <a href="#server-setwritehook" class="reference internal">server:setWriteHook</a>
      - <a href="#writeHook" class="reference internal"><span class="pre"><code class="docutils literal notranslate">writeHook()</code></span></a>
    - <a href="#server-loadxmlmodels" class="reference internal">server:loadXmlModels</a>
    - <a href="#server-createnamespace" class="reference internal">server:createNamespace</a>
    - <a href="#server-exportxmlmodels" class="reference internal">server:exportXmlModels</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Server](index.html)
- Server API
- <a href="../_sources/server/server_api.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="server-api" class="section">

# Server API<a href="#server-api" class="headerlink" title="Link to this heading"></a>

<div id="server-constructor" class="section">

## Server Constructor<a href="#server-constructor" class="headerlink" title="Link to this heading"></a>

To create a server, call <span class="pre">`ua.newServer`</span> from the <span class="pre">`opcua.api`</span> module.

<span class="sig-prename descclassname"><span class="pre">ua.</span></span><span class="sig-name descname"><span class="pre">newServer</span></span><span class="sig-paren">(</span><span class="optional">\[</span>*<span class="n"><span class="pre">config</span></span>*<span class="optional">\]</span><span class="optional">\[</span>, *<span class="n"><span class="pre">model</span></span>*<span class="optional">\]</span><span class="sig-paren">)</span><a href="#ua.newServer" class="headerlink" title="Link to this definition"></a>  
Config<span class="colon">:</span>  
table with <a href="server_config.html#configuration-table" class="reference internal"><span class="std std-ref">Configuration table</span></a> options.

If not provided, the default configuration will be used. In this case, the host name for the endpoint is detected automatically. The server listens on port <span class="pre">`4841`</span>.

Model<span class="colon">:</span>  
<a href="../model/index.html#address-space-api" class="reference internal"><span class="std std-ref">Address Space API</span></a> to use.  
If **nil** then <a href="../model/model.html#ua.baseModel" class="reference internal" title="ua.baseModel"><span class="pre"><code class="sourceCode python">ua.baseModel()</code></span></a> will be initialized.

<div class="highlight-lua notranslate">

<div class="highlight">

    config = {
      bufSize = 16384,
      securePolicies ={
        {
          securityPolicyUri = "http://opcfoundation.org/UA/SecurityPolicy#None"
        }
      }
      endpointUrl = "opc.tcp://[hostname|ip]:4841"
    }

</div>

</div>

Returns<span class="colon">:</span>  
server object

<div id="server-default-configuration" class="section">

### Server default configuration<a href="#server-default-configuration" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Load the OPCUA API module
    local ua = require("opcua.api")

    -- Create new OPC UA server instance.
    -- Pass configuration table to server.
    local server = ua.newServer()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_getting_started.lua" class="reference external">Full source</a>

</div>

<div id="server-custom-configuration" class="section">

### Server custom configuration<a href="#server-custom-configuration" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    local ua = require("opcua.api")
    local config = {
      endpoints = {
        {
          endpointUrl = "opc.tcp://localhost:4845",
        }
      },

      securePolicies = {
       { -- #1
         securityPolicyUri = ua.SecurityPolicy.None,
       },
     },
    }


    local server = ua.newServer(config)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_value_callback.lua" class="reference external">Full source</a>

</div>

</div>

<div id="server-initialize" class="section">

## server:initialize<a href="#server-initialize" class="headerlink" title="Link to this heading"></a>

<span class="sig-name descname"><span class="pre">server:initialize(\[initAddons\])</span></span>  
This method initializes internal server structures and becomes ready for customization: adding nodes, setting variable sources, writing values, etc.

InitAddons<span class="colon">:</span>  
Optional function called with the internal services object after services are created and started.

Returns<span class="colon">:</span>  
nil

</div>

<div id="server-run" class="section">

## server:run<a href="#server-run" class="headerlink" title="Link to this heading"></a>

This method opens a server socket and starts listening for incoming connections.

<span class="sig-name descname"><span class="pre">server:run()</span></span>  
Returns<span class="colon">:</span>  
error

</div>

<div id="server-shutdown" class="section">

## server:shutdown<a href="#server-shutdown" class="headerlink" title="Link to this heading"></a>

This method stops the server and closes all connections.

<span class="sig-name descname"><span class="pre">server:shutdown()</span></span>  
Returns<span class="colon">:</span>  
nil

</div>

<div id="server-addnodes" class="section">

## server:addNodes<a href="#server-addnodes" class="headerlink" title="Link to this heading"></a>

Add new nodes to server address space.

<span class="sig-name descname"><span class="pre">server:addNodes(parameters)</span></span>  
Parameters<span class="colon">:</span>  
A table with a field NodesToAdd. NodesToAdd is an array of tables with the parameters of new nodes. For details see <a href="../client/adding_nodes.html#adding-nodes" class="reference internal"><span class="std std-ref">Adding Nodes</span></a>

Returns<span class="colon">:</span>  
The result from an OPC UA call will be an array. Every element of the array will be a table with two fields: Status code for the current node and identifier of the added node.

Every element of the array contains a table with the following fields:

**StatusCode**  
The status code from adding the corresponding node.

**AddedNodeId**  
Identifier of added node. The server automatically assigns an identifier for the node if not included. The identifier will be nil in case of error.

</div>

<div id="server-browse" class="section">

## server:browse<a href="#server-browse" class="headerlink" title="Link to this heading"></a>

Browse nodes in the server address space.

<span class="sig-name descname"><span class="pre">server:browse(\<NodeId</span> <span class="pre">\|</span> <span class="pre">NodeId\[\]</span> <span class="pre">\|</span> <span class="pre">parameters\>)</span></span>  
NodeId<span class="colon">:</span>  
Browse one node by NodeId.

NodeId\[\]<span class="colon">:</span>  
Array of NodeIds to browse.

Parameters<span class="colon">:</span>  
Table with detailed parameters. For details see <a href="../client/browsing.html#browsing-detailed-parameters" class="reference internal"><span class="std std-ref">Detailed Parameters</span></a>

</div>

<div id="server-read" class="section">

## server:read<a href="#server-read" class="headerlink" title="Link to this heading"></a>

Read attribute values of nodes.

<span class="sig-name descname"><span class="pre">server:read(\<NodeId</span> <span class="pre">\|</span> <span class="pre">NodeId\[\]</span> <span class="pre">\|</span> <span class="pre">parameters\>)</span></span>  
NodeId<span class="colon">:</span>  
(<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>) Read the <span class="pre">`Value`</span> attribute of one node by NodeId.

NodeId\[\]<span class="colon">:</span>  
(<a href="../types.html#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>) Array of NodeIds to read. The <span class="pre">`Value`</span> attribute will be read for each NodeId.

Parameters<span class="colon">:</span>  
(table) Table with detailed parameters. For details see <a href="../client/read_write_data.html#read-attributes" class="reference internal"><span class="std std-ref">Reading Attributes</span></a>

Returns<span class="colon">:</span>  
The result from an OPC UA call will be an array. Every element of the array will be a table with two fields: Status code for the current node and the value of the attribute.

Every element of the array contains a table with the following fields:

**StatusCode**  
The status code from reading the corresponding node.

**Value** (<a href="../types.html#data-value-type" class="reference internal"><span class="std std-ref">DataValue</span></a>)  
The value of the attribute. The value will be nil in case of error.

</div>

<div id="server-write" class="section">

## server:write<a href="#server-write" class="headerlink" title="Link to this heading"></a>

<span class="sig-name descname"><span class="pre">server:write(parameters)</span></span>  
Parameters<span class="colon">:</span>  
A table with a field NodesToWrite. NodesToWrite is an array of tables with the parameters of nodes to write. For details see <a href="../client/read_write_data.html#writing-attributes" class="reference internal"><span class="std std-ref">Writing Attributes</span></a>

Returns<span class="colon">:</span>  
response, error

**response**  
*array* if error is *nil* The result from an OPC UA call will be an array. Every element of the array is Status code for each node

*nil* if *StatusCode* is not nil

**error**  
StatusCode of operation overall or nil

</div>

<div id="server-setvaluecallback" class="section">

## server:setValueCallback<a href="#server-setvaluecallback" class="headerlink" title="Link to this heading"></a>

This method sets a callback function for a variable node. The callback function will be called when the value of the variable is reading or writing. For more details, see <a href="data_source.html#custom-data-source" class="reference internal"><span class="std std-ref">Exporting Device Data</span></a>

<span class="sig-name descname"><span class="pre">server:setValueCallback(nodeId,</span> <span class="pre">callback)</span></span>  
NodeId<span class="colon">:</span>  
NodeId of the variable node.

Callback<span class="colon">:</span>  
Function to be called when the value of the variable is being read or written.

</div>

<div id="server-setwritehook" class="section">

## server:setWriteHook<a href="#server-setwritehook" class="headerlink" title="Link to this heading"></a>

This method sets a callback function for a variable node. When the value of the variable has been changed, the callback function will be called.

<span class="sig-name descname"><span class="pre">server:setWriteHook(nodeId,</span> <span class="pre">writeHook)</span></span>  
NodeId<span class="colon">:</span>  
NodeId for which to set a hook.

WriteHook<span class="colon">:</span>  
Function to be called when the value of the variable has changed.

<!-- -->

<span class="sig-name descname"><span class="pre">writeHook</span></span><span class="sig-paren">(</span>*<span class="n"><span class="pre">nodeId</span></span>*, *<span class="n"><span class="pre">attributeId</span></span>*, *<span class="n"><span class="pre">value</span></span>*<span class="sig-paren">)</span><a href="#writeHook" class="headerlink" title="Link to this definition"></a>  
NodeId<span class="colon">:</span>  
NodeId whose attribute has changed.

AttributeId<span class="colon">:</span>  
AttributeId that changed.

Value<span class="colon">:</span>  
New value of the attribute.

</div>

<div id="server-loadxmlmodels" class="section">

## server:loadXmlModels<a href="#server-loadxmlmodels" class="headerlink" title="Link to this heading"></a>

Load XML models to server address space.

<span class="sig-name descname"><span class="pre">server:loadXmlModels(modelFiles)</span></span>  
ModelFiles<span class="colon">:</span>  
Array of XML model paths, HTTP/HTTPS URLs, or XML document strings.

All nodes from the same XML model are added to the same namespace. For every new model, a new namespace is created with the next index after the last added namespace. Namespace index 0 is reserved for the default namespace ‘<a href="http://opcfoundation.org/UA/" class="reference external">http://opcfoundation.org/UA/</a>’. Namespace index 1 is reserved for the namespace reserved for the current server. All new loaded models will be mapped to namespaces with indexes starting from 2.

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Load the required modules
    local ua = require("opcua.api")

    local server = ua.newServer()
    server:initialize()

    -- current file path
    local rootPath = debug.getinfo(1, "S").short_src:match("..(/.*/)")
    rootPath = ba.openio("home"):realpath(rootPath)

    server:loadXmlModels({
      rootPath.."euromap83_1_03/Opc.Ua.Di.NodeSet2.xml",
      rootPath.."euromap83_1_03/Opc.Ua.PlasticsRubber.GeneralTypes.NodeSet2.xml"
    })

    server:run()
    server:shutdown()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_load_models_file.lua" class="reference external">Full source</a>

</div>

<div id="server-createnamespace" class="section">

## server:createNamespace<a href="#server-createnamespace" class="headerlink" title="Link to this heading"></a>

Create a new namespace.

<span class="sig-name descname"><span class="pre">server:createNamespace(namespaceUri)</span></span>  
NamespaceUri<span class="colon">:</span>  
URI of the namespace.

Returns<span class="colon">:</span>  
index of the namespace.

This method is used to create a new namespace. The created namespace will have the next index after the last added namespace. Namespace index 0 is reserved for the default namespace ‘<a href="http://opcfoundation.org/UA/" class="reference external">http://opcfoundation.org/UA/</a>’. Namespace index 1 is reserved for the namespace reserved for the current server. All newly created namespaces will be mapped to namespaces with indexes starting from 2.

<div class="highlight-lua notranslate">

<div class="highlight">

    local server = ua.newServer()
    server:initialize()

    local nsIndex = server:createNamespace("http://test.com")
    print(nsIndex)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_create_namespace.lua" class="reference external">Full source</a>

</div>

<div id="server-exportxmlmodels" class="section">

## server:exportXmlModels<a href="#server-exportxmlmodels" class="headerlink" title="Link to this heading"></a>

Export XML models to the file.

<span class="sig-name descname"><span class="pre">server:exportXmlModels(output,</span> <span class="pre">namespaceUris)</span></span>  
Output<span class="colon">:</span>  
File path or callback function to export XML models.

NamespaceUris<span class="colon">:</span>  
Array of namespace URIs to export.

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Load XML models from the OPCUA Foundation github repository
    local baseUrl = "https://raw.githubusercontent.com/OPCFoundation/UA-Nodeset/refs/heads/latest"
    server:loadXmlModels({
      baseUrl.."/DI/Opc.Ua.Di.NodeSet2.xml",
      baseUrl.."/PlasticsRubber/GeneralTypes/1.03/Opc.Ua.PlasticsRubber.GeneralTypes.NodeSet2.xml"
    })

    -- Method 1: Export all models to a file
    local outputFile = "exported_models.xml"
    server:exportXmlModels(outputFile)

    print("Exported all models to: " .. outputFile)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_export_model_xml.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="http.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="HTTP Server"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="../pubsub/index.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Publish Subscribe API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/server/server_config.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="index.html" class="reference internal">Server</a>
  - <a href="getting_started.html" class="reference internal">Getting Started with Server</a>
  - <a href="#" class="current reference internal">Configuration table</a>
  - <a href="data_source.html" class="reference internal">Exporting Device Data</a>
  - <a href="xml_models.html" class="reference internal">XML Models</a>
  - <a href="authentication.html" class="reference internal">Server side authentication</a>
  - <a href="init_opcua_project.html" class="reference internal">OPC UA Server Project Code Generator</a>
  - <a href="http.html" class="reference internal">HTTP Server</a>
  - <a href="server_api.html" class="reference internal">Server API</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Server](index.html)
- Configuration table
- <a href="../_sources/server/server_config.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="configuration-table" class="section">


# Configuration table<a href="#configuration-table" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    local configuration = {
      endpoints = {
        {
          -- TCP port number the server will be listening on.
          listenPort=4841,

          -- Optionally bind to a specific network interface.
          -- The value '*' means listen on all interfaces.
          listenAddress="localhost",

          -- Optionally set Endpoint URL, the URL that will be returned to
          -- clients when they call GetEndpoints. This parameter may differ
          -- from listenAddress. This is useful when, for example, the server
          -- is working behind a load balancer (inside Kubernetes or
          -- Docker). In this case, the server will return the DNS name of
          -- the site instead of the host name of the container.
          endpointUrl="opc.tcp://realtimelogic.com:4841",
        }

        -- HTTP based endpoints
        -- opc.http(s):// schemes are defined in OPC UA specification,
        -- but may not be supported by all clients.
        {
          endpointUrl = "opc.http://localhost:9357/opcua",
        },
        {
          endpointUrl = "opc.https://localhost:9357/opcua",
        },

        -- http(s):// schemes are not defined in OPC UA specification
        -- and can be specified for clients that do not support 'opc.http'
        -- schemes.
        {
          endpointUrl = "http://localhost:9357/opcua",
        },
        {
          endpointUrl = "https://localhost:9357/opcua",
        },
      },


      -- This is the main certificate and key for the server.
      -- It is used in endpoint description, during asymmetric encryption
      -- and for signing during session authentication process.
      certificate = "/path/to/server/main.pem",
      key = "/path/to/server/main.key",

      -- List of secure policies that can be applied to messages
      -- To disable any policy, remove the required entry from the list
      securePolicies = {
        { -- #1
          -- Insecure policy.
          -- Use this policy for testing purposes only.
          securityPolicyUri = ua.SecurityPolicy.None,
        },

        { -- #2
          -- Secure policy Basic128Rsa15
          -- Asymmetric:
          -- RSA key size 1024 or 2048 bits.
          -- AES key size 128bit
          securityPolicyUri = ua.SecurityPolicy.Basic128Rsa15,

          -- Secure mode sign and sign-and-encrypt
          -- You can leave only one mode
          securityMode = {
            ua.MessageSecurityMode.Sign,
            ua.MessageSecurityMode.SignAndEncrypt
          },

          -- certificate and private key should be used with secure policy.
          -- 1. Path to files
          -- 2. Content of certificate/key.
          -- If these fields are not specified, then the main certificate will be used.
          certificate = "/path/to/certs/basic128rsa15.pem",
          key =         "/path/to/certs/basic128rsa15.key",
        },
        { -- #3
          -- Secure policy Aes128_Sha256_RsaOaep
          -- Asymmetric:
          -- RSA key size 1024 or 2048 bits.
          -- AES key size 128bit
          securityPolicyUri = ua.SecurityPolicy.Aes128_Sha256_RsaOaep,

          -- Secure mode sign and sign-and-encrypt
          -- You can leave only one mode
          securityMode = {
            ua.MessageSecurityMode.Sign,
            ua.MessageSecurityMode.SignAndEncrypt
          },

          -- certificate and private key should be used with secure policy.
          -- 1. Path to files
          -- 2. Content of certificate/key.
          -- If these fields are not specified, then the main certificate will be used.
          certificate = "/path/to/certs/Aes128_Sha256_RsaOaep.pem",
          key =         "/path/to/certs/Aes128_Sha256_RsaOaep.key",
        }
      },

      -- Size of the buffer used for encoding/decoding messages.
      -- Cannot be less than 8192.
      bufSize = 16384,

      -- Log settings. If all parameters are false, then server will be
      -- working in silent mode without producing logs.
      logging = {
        -- Trace information messages specific to sockets.
        socket = {
          -- Show data sent over sockets. Produces lots of data
          dbgOn = false,
          -- Client connect and disconnect information
          infOn = false,
          -- Socket errors.
          errOn = true
        },
        -- For OPC UA binary protocol experts:
        binary = {
          -- Enable debugging of binary OPC UA protocol:
          --   * What kind of message received
          --   * Encoding/decoding information
          --   * What services are called
          --   * Tokens refresh process
          dbgOn = false,

          -- Information messages:
          --  * Number of channels created.
          --  * Issued and expired token numbers.
          infOn = true,

          -- Binary protocol errors
          errOn = true
        },

        services = {
          -- Service execution information.
          dbgOn = true,
          -- Service message information.
          infOn = true,
          -- Service errors.
          errOn = true
        }
      }
    }

</div>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="getting_started.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Getting Started with Server"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="data_source.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Exporting Device Data">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/server/xml_models.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="../about.html" class="reference internal">About opcua-lua</a>
- <a href="../opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="../model/index.html" class="reference internal">Address Space API</a>
- <a href="../client/index.html" class="reference internal">Client</a>
- <a href="index.html" class="reference internal">Server</a>
  - <a href="getting_started.html" class="reference internal">Getting Started with Server</a>
  - <a href="server_config.html" class="reference internal">Configuration table</a>
  - <a href="data_source.html" class="reference internal">Exporting Device Data</a>
  - <a href="#" class="current reference internal">XML Models</a>
    - <a href="#loading-xml-models" class="reference internal">Loading XML models</a>
    - <a href="#export-xml-models" class="reference internal">Export XML models</a>
  - <a href="authentication.html" class="reference internal">Server side authentication</a>
  - <a href="init_opcua_project.html" class="reference internal">OPC UA Server Project Code Generator</a>
  - <a href="http.html" class="reference internal">HTTP Server</a>
  - <a href="server_api.html" class="reference internal">Server API</a>
- <a href="../pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="../integrations/index.html" class="reference internal">Examples</a>
- <a href="../logging.html" class="reference internal">Logging subsystem</a>
- <a href="../secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="../application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="../types.html" class="reference internal">Data Types</a>
- <a href="../interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- [Server](index.html)
- XML Models
- <a href="../_sources/server/xml_models.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="xml-models" class="section">

# XML Models<a href="#xml-models" class="headerlink" title="Link to this heading"></a>

XML models are used to describe the structure of OPC UA address space: its nodes, types, variables, etc. There are many predefined XML models in the OPC UA. You can find them in the git repository <a href="https://github.com/OPCFoundation/UA-Nodeset/tree/latest/" class="reference external">NodeSet</a>.

<div id="loading-xml-models" class="section">

## Loading XML models<a href="#loading-xml-models" class="headerlink" title="Link to this heading"></a>

When you start the server, you can load XML models from the file. During loading, the server will parse the XML models and build the address space. For every new loaded XML model added to the server, the server will create a new namespace.

Consider loading XML model for plastic rubber industry: <a href="https://github.com/OPCFoundation/UA-Nodeset/tree/latest/PlasticsRubber" class="reference external">PlasticRubber</a>.

The following example shows how to load the XML models by using the HTTP protocol from the OPC UA Foundation GitHub repository:

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Load the required modules
    local ua = require("opcua.api")

    local server = ua.newServer()
    server:initialize()

    local baseUrl = "https://raw.githubusercontent.com/OPCFoundation/UA-Nodeset/refs/heads/latest"
    server:loadXmlModels({
      baseUrl.."/DI/Opc.Ua.Di.NodeSet2.xml",
      baseUrl.."/PlasticsRubber/GeneralTypes/1.03/Opc.Ua.PlasticsRubber.GeneralTypes.NodeSet2.xml"
    })

    server:run()
    server:shutdown()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_load_models_http.lua" class="reference external">Full source</a>

The following example shows how to load the XML models from the local file:

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Load the required modules
    local ua = require("opcua.api")

    local server = ua.newServer()
    server:initialize()

    -- current file path
    local rootPath = debug.getinfo(1, "S").short_src:match("..(/.*/)")
    rootPath = ba.openio("home"):realpath(rootPath)

    server:loadXmlModels({
      rootPath.."euromap83_1_03/Opc.Ua.Di.NodeSet2.xml",
      rootPath.."euromap83_1_03/Opc.Ua.PlasticsRubber.GeneralTypes.NodeSet2.xml"
    })

    server:run()
    server:shutdown()

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_load_models_file.lua" class="reference external">Full source</a>

</div>

<div id="export-xml-models" class="section">

## Export XML models<a href="#export-xml-models" class="headerlink" title="Link to this heading"></a>

You can export XML models from the server to the file. It is possible to export either all models or selected models by specifying namespace URIs. The following example shows how to export XML models to the file:

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Load XML models from the OPCUA Foundation github repository
    local baseUrl = "https://raw.githubusercontent.com/OPCFoundation/UA-Nodeset/refs/heads/latest"
    server:loadXmlModels({
      baseUrl.."/DI/Opc.Ua.Di.NodeSet2.xml",
      baseUrl.."/PlasticsRubber/GeneralTypes/1.03/Opc.Ua.PlasticsRubber.GeneralTypes.NodeSet2.xml"
    })

    -- Method 1: Export all models to a file
    local outputFile = "exported_models.xml"
    server:exportXmlModels(outputFile)

    print("Exported all models to: " .. outputFile)

</div>

</div>

<a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/server/server_export_model_xml.lua" class="reference external">Full source</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="data_source.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Exporting Device Data"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="authentication.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Server side authentication">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/types.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Documentation</span>

- <a href="about.html" class="reference internal">About opcua-lua</a>
- <a href="opcua_overview.html" class="reference internal">OPC UA overview</a>
- <a href="model/index.html" class="reference internal">Address Space API</a>
- <a href="client/index.html" class="reference internal">Client</a>
- <a href="server/index.html" class="reference internal">Server</a>
- <a href="pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="integrations/index.html" class="reference internal">Examples</a>
- <a href="logging.html" class="reference internal">Logging subsystem</a>
- <a href="secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="#" class="current reference internal">Data Types</a>
  - <a href="#built-in-types" class="reference internal">Built-in Types</a>
  - <a href="#localizedtext" class="reference internal">LocalizedText</a>
  - <a href="#qualifiedname" class="reference internal">QualifiedName</a>
  - <a href="#nodeid" class="reference internal">NodeId</a>
    - <a href="#nodeid-helpers" class="reference internal">NodeId helpers</a>
  - <a href="#variant" class="reference internal">Variant</a>
    - <a href="#varianttype-constants" class="reference internal">VariantType constants</a>
  - <a href="#datavalue" class="reference internal">DataValue</a>
  - <a href="#extensionobject" class="reference internal">ExtensionObject</a>
  - <a href="#service-structures" class="reference internal">Service Structures</a>
    - <a href="#activatesessionresponse" class="reference internal">ActivateSessionResponse</a>
    - <a href="#addnodesresponse" class="reference internal">AddNodesResponse</a>
    - <a href="#browseparameters" class="reference internal">BrowseParameters</a>
    - <a href="#browseresult" class="reference internal">BrowseResult</a>
    - <a href="#closesecurechannelresponse" class="reference internal">CloseSecureChannelResponse</a>
    - <a href="#closesessionresponse" class="reference internal">CloseSessionResponse</a>
    - <a href="#createsessionresponse" class="reference internal">CreateSessionResponse</a>
    - <a href="#findserversresponse" class="reference internal">FindServersResponse</a>
    - <a href="#getendpointsresponse" class="reference internal">GetEndpointsResponse</a>
    - <a href="#opensecurechannelresponse" class="reference internal">OpenSecureChannelResponse</a>
    - <a href="#readresponse" class="reference internal">ReadResponse</a>
    - <a href="#writeresponse" class="reference internal">WriteResponse</a>
- <a href="interoperability_notes.html" class="reference internal">OPC UA interoperability notes</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="index.html" class="icon icon-home" aria-label="Home"></a>
- Data Types
- <a href="_sources/types.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="data-types" class="section">

# Data Types<a href="#data-types" class="headerlink" title="Link to this heading"></a>

This page summarizes the Lua representations used by the OPC UA stack. Values are ordinary Lua values unless a table structure is shown.

<div id="built-in-types" class="section">

## Built-in Types<a href="#built-in-types" class="headerlink" title="Link to this heading"></a>

<span id="guid"></span><span id="bytestring"></span><span id="string"></span><span id="datetime"></span><span id="double"></span><span id="float"></span><span id="uint64"></span><span id="int64"></span><span id="uint32"></span><span id="int32"></span><span id="uint16"></span><span id="int16"></span><span id="byte"></span><span id="sbyte"></span>

| OPC UA type                           | Lua representation | Notes                                                                                                                                                                         |
|---------------------------------------|--------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <span class="pre">`SByte`</span>      | number             | Signed 8-bit integer.                                                                                                                                                         |
| <span class="pre">`Byte`</span>       | number             | Unsigned 8-bit integer.                                                                                                                                                       |
| <span class="pre">`Int16`</span>      | number             | Signed 16-bit integer.                                                                                                                                                        |
| <span class="pre">`UInt16`</span>     | number             | Unsigned 16-bit integer.                                                                                                                                                      |
| <span class="pre">`Int32`</span>      | number             | Signed 32-bit integer.                                                                                                                                                        |
| <span class="pre">`UInt32`</span>     | number             | Unsigned 32-bit integer.                                                                                                                                                      |
| <span class="pre">`Int64`</span>      | number             | Signed 64-bit integer.                                                                                                                                                        |
| <span class="pre">`UInt64`</span>     | number             | Unsigned 64-bit integer.                                                                                                                                                      |
| <span class="pre">`Float`</span>      | number             | 32-bit floating-point value.                                                                                                                                                  |
| <span class="pre">`Double`</span>     | number             | 64-bit floating-point value.                                                                                                                                                  |
| <span class="pre">`DateTime`</span>   | number             | Unix timestamp in seconds, usually from <span class="pre">`compat.gettime()`</span> or <span class="pre">`os.time()`</span>. Encoders convert this to the OPC UA wire format. |
| <span class="pre">`String`</span>     | string             | UTF-8 string.                                                                                                                                                                 |
| <span class="pre">`ByteString`</span> | string             | Binary byte sequence stored in a Lua string.                                                                                                                                  |
| <span class="pre">`Guid`</span>       | string             | GUID string, for example <span class="pre">`12345678-1234-1234-1234-123456789012`</span>.                                                                                     |
| <span class="pre">`StatusCode`</span> | number             | OPC UA status code, for example <span class="pre">`ua.StatusCode.Good`</span>.                                                                                                |

</div>

<div id="localizedtext" class="section">


## LocalizedText<a href="#localizedtext" class="headerlink" title="Link to this heading"></a>

Localized text is represented as a table.

| Field                             | Type             | Description                                                                                     |
|-----------------------------------|------------------|-------------------------------------------------------------------------------------------------|
| <span class="pre">`Locale`</span> | string, optional | Locale name such as <span class="pre">`"en-US"`</span>. If omitted, the default locale is used. |
| <span class="pre">`Text`</span>   | string           | Localized text content.                                                                         |

<div class="highlight-lua notranslate">

<div class="highlight">

    local text = {Locale = "en-US", Text = "This is localized text"}
    local textDefaultLocale = {Text = "This is localized text"}

</div>

</div>

</div>

<div id="qualifiedname" class="section">


## QualifiedName<a href="#qualifiedname" class="headerlink" title="Link to this heading"></a>

A QualifiedName is the service identifier of a node. It combines a namespace index with a name.

| Field                           | Type   | Description                |
|---------------------------------|--------|----------------------------|
| <span class="pre">`ns`</span>   | number | Namespace index.           |
| <span class="pre">`Name`</span> | string | Name within the namespace. |

<div class="highlight-lua notranslate">

<div class="highlight">

    local qualifiedName = {ns = 1, Name = "Node Name"}

</div>

</div>

</div>

<div id="nodeid" class="section">


## NodeId<a href="#nodeid" class="headerlink" title="Link to this heading"></a>

A NodeId is encoded as a string:

<div class="highlight-text notranslate">

<div class="highlight">

    [ns=<namespace_index>;]<type>=<value>

</div>

</div>

| Part                                       | Value                                                                                                                     | Description                                                              |
|--------------------------------------------|---------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------|
| <span class="pre">`namespace_index`</span> | unsigned 16-bit integer                                                                                                   | Optional. Omit when the namespace index is <span class="pre">`0`</span>. |
| <span class="pre">`type`</span>            | <span class="pre">`i`</span>, <span class="pre">`s`</span>, <span class="pre">`g`</span>, or <span class="pre">`b`</span> | Identifier type: integer, string, GUID, or opaque byte string.           |
| <span class="pre">`value`</span>           | type-specific string                                                                                                      | Decimal integer, UTF-8 string, lowercase GUID, or base64 byte string.    |

| Type code                    | Identifier type    | Example                                                                   |
|------------------------------|--------------------|---------------------------------------------------------------------------|
| <span class="pre">`i`</span> | Integer            | <span class="pre">`i=85`</span> or <span class="pre">`ns=2;i=1001`</span> |
| <span class="pre">`s`</span> | String             | <span class="pre">`ns=2;s=Temperature`</span>                             |
| <span class="pre">`g`</span> | GUID               | <span class="pre">`ns=2;g=12345678-1234-1234-1234-123456789012`</span>    |
| <span class="pre">`b`</span> | Opaque byte string | <span class="pre">`ns=2;b=AQIDBA==`</span>                                |

Additional examples are available in <a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/OPC-UA/node_id.lua" class="reference external">node_id.lua</a>. The official syntax is described in the OPC Foundation <a href="https://reference.opcfoundation.org/v104/Core/docs/Part6/5.3.1/#5.3.1.10" class="reference external">NodeId reference</a>.

<div id="nodeid-helpers" class="section">

### NodeId helpers<a href="#nodeid-helpers" class="headerlink" title="Link to this heading"></a>

| Function                                                                                                                                                                                                        | Parameters                                                                                                                                                           | Return value                                                                       |
|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------|
| <span class="pre">`toString(id,`</span>` `<span class="pre">`ns)`</span>                                                                                                                                        | <span class="pre">`id`</span> is a number, string, GUID, byte array, or bytes. <span class="pre">`ns`</span> is a namespace index or <span class="pre">`nil`</span>. | Encoded NodeId string.                                                             |
| <span class="pre">`toString({id`</span>` `<span class="pre">`=`</span>` `<span class="pre">`value,`</span>` `<span class="pre">`ns`</span>` `<span class="pre">`=`</span>` `<span class="pre">`number})`</span> | Table form of the same parameters.                                                                                                                                   | Encoded NodeId string.                                                             |
| <span class="pre">`fromString(string)`</span>                                                                                                                                                                   | Encoded NodeId string.                                                                                                                                               | Table with <span class="pre">`ns`</span> and <span class="pre">`id`</span> fields. |

</div>

</div>

<div id="variant" class="section">


## Variant<a href="#variant" class="headerlink" title="Link to this heading"></a>

A Variant wraps a value with an OPC UA type. It can represent a scalar or an array.

| Field                                      | Type                                      | Description                                                                               |
|--------------------------------------------|-------------------------------------------|-------------------------------------------------------------------------------------------|
| <span class="pre">`Type`</span>            | <span class="pre">`ua.VariantType`</span> | Type of the stored value.                                                                 |
| <span class="pre">`Value`</span>           | any supported Lua value                   | Value to encode.                                                                          |
| <span class="pre">`IsArray`</span>         | boolean, optional                         | Set to <span class="pre">`true`</span> when <span class="pre">`Value`</span> is an array. |
| <span class="pre">`ArrayDimensions`</span> | number array, optional                    | Dimensions for array values.                                                              |

<div class="highlight-lua notranslate">

<div class="highlight">

    local uint32 = {Type = ua.VariantType.UInt32, Value = 0}
    local uint32Array = {
      Type = ua.VariantType.UInt32,
      Value = {1, 2, 3, 4},
      IsArray = true,
      ArrayDimensions = {4}
    }

</div>

</div>

<div id="varianttype-constants" class="section">

### VariantType constants<a href="#varianttype-constants" class="headerlink" title="Link to this heading"></a>

| Name                                       | Value | Name                                      | Value |
|--------------------------------------------|-------|-------------------------------------------|-------|
| <span class="pre">`Null`</span>            | 0     | <span class="pre">`Boolean`</span>        | 1     |
| <span class="pre">`SByte`</span>           | 2     | <span class="pre">`Byte`</span>           | 3     |
| <span class="pre">`Int16`</span>           | 4     | <span class="pre">`UInt16`</span>         | 5     |
| <span class="pre">`Int32`</span>           | 6     | <span class="pre">`UInt32`</span>         | 7     |
| <span class="pre">`Int64`</span>           | 8     | <span class="pre">`UInt64`</span>         | 9     |
| <span class="pre">`Float`</span>           | 10    | <span class="pre">`Double`</span>         | 11    |
| <span class="pre">`String`</span>          | 12    | <span class="pre">`DateTime`</span>       | 13    |
| <span class="pre">`Guid`</span>            | 14    | <span class="pre">`ByteString`</span>     | 15    |
| <span class="pre">`XmlElement`</span>      | 16    | <span class="pre">`NodeId`</span>         | 17    |
| <span class="pre">`ExpandedNodeId`</span>  | 18    | <span class="pre">`StatusCode`</span>     | 19    |
| <span class="pre">`QualifiedName`</span>   | 20    | <span class="pre">`LocalizedText`</span>  | 21    |
| <span class="pre">`ExtensionObject`</span> | 22    | <span class="pre">`DataValue`</span>      | 23    |
| <span class="pre">`Variant`</span>         | 24    | <span class="pre">`DiagnosticInfo`</span> | 25    |

</div>

</div>

<div id="datavalue" class="section">


## DataValue<a href="#datavalue" class="headerlink" title="Link to this heading"></a>

A DataValue is a Variant value plus optional status and timestamp fields.

| Field                                        | Type                                      | Description                                                              |
|----------------------------------------------|-------------------------------------------|--------------------------------------------------------------------------|
| <span class="pre">`Type`</span>              | <span class="pre">`ua.VariantType`</span> | Type of <span class="pre">`Value`</span>.                                |
| <span class="pre">`Value`</span>             | any supported Lua value                   | Value to encode.                                                         |
| <span class="pre">`StatusCode`</span>        | status code, optional                     | Value status, for example <span class="pre">`ua.StatusCode.Good`</span>. |
| <span class="pre">`SourceTimestamp`</span>   | DateTime, optional                        | Source timestamp.                                                        |
| <span class="pre">`ServerTimestamp`</span>   | DateTime, optional                        | Server timestamp.                                                        |
| <span class="pre">`SourcePicoseconds`</span> | UInt16, optional                          | Fractional source timestamp precision.                                   |
| <span class="pre">`ServerPicoseconds`</span> | UInt16, optional                          | Fractional server timestamp precision.                                   |

<div class="highlight-lua notranslate">

<div class="highlight">

    local dataValue = {
      Type = ua.VariantType.UInt32,
      Value = 100,
      StatusCode = ua.StatusCode.Good,
      SourceTimestamp = os.time()
    }

</div>

</div>

</div>

<div id="extensionobject" class="section">


## ExtensionObject<a href="#extensionobject" class="headerlink" title="Link to this heading"></a>

An ExtensionObject represents a structured value. It is also used when a field can contain one of several structure types, such as node attributes for <span class="pre">`AddNodes`</span>.

| Field                             | Type                                                                                           | Description                                                                                                                 |
|-----------------------------------|------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------|
| <span class="pre">`TypeId`</span> | <a href="#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> | NodeId of the structure type. This is the decoded structure NodeId, not the binary, XML, or JSON encoding NodeId.           |
| <span class="pre">`Body`</span>   | table, ByteString, XML string, or JSON string                                                  | Decoded body table when the structure is known. Otherwise, the body is left opaque and must be decoded by application code. |

<div class="highlight-lua notranslate">

<div class="highlight">

    local extensionObject = {
      TypeId = "i=338", -- BuildInfo
      Body = {
        ProductUri = ua.Version.ProductUri,
        ManufacturerName = ua.Version.ManufacturerName,
        ProductName = ua.Version.ProductName,
        SoftwareVersion = ua.Version.Version,
        BuildNumber = ua.Version.BuildNumber,
        BuildDate = compat.gettime()
      }
    }

</div>

</div>

</div>

<div id="service-structures" class="section">

## Service Structures<a href="#service-structures" class="headerlink" title="Link to this heading"></a>

<div id="activatesessionresponse" class="section">

### ActivateSessionResponse<a href="#activatesessionresponse" class="headerlink" title="Link to this heading"></a>

| Field                                  | Type             | Description                             |
|----------------------------------------|------------------|-----------------------------------------|
| <span class="pre">`ServerNonce`</span> | ByteString       | Random value that should not be reused. |
| <span class="pre">`Results`</span>     | StatusCode array | User identity token validation results. |

</div>

<div id="addnodesresponse" class="section">

### AddNodesResponse<a href="#addnodesresponse" class="headerlink" title="Link to this heading"></a>

| Field                                            | Type                                                                                           | Description                                                                        |
|--------------------------------------------------|------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------|
| <span class="pre">`Results[]`</span>             | array                                                                                          | One result per requested node.                                                     |
| <span class="pre">`Results[].StatusCode`</span>  | StatusCode                                                                                     | Status of the add operation.                                                       |
| <span class="pre">`Results[].AddedNodeId`</span> | <a href="#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> | Server-assigned NodeId, or <span class="pre">`nil`</span> if the operation failed. |

</div>

<div id="browseparameters" class="section">

### BrowseParameters<a href="#browseparameters" class="headerlink" title="Link to this heading"></a>

| Field                                                      | Type                                                                                           | Description                                                                           |
|------------------------------------------------------------|------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------|
| <span class="pre">`RequestedMaxReferencesPerNode`</span>   | UInt32                                                                                         | Maximum number of references to return. <span class="pre">`0`</span> means unlimited. |
| <span class="pre">`NodesToBrowse[]`</span>                 | array                                                                                          | Nodes to browse.                                                                      |
| <span class="pre">`NodesToBrowse[].NodeId`</span>          | <a href="#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> | Node to browse.                                                                       |
| <span class="pre">`NodesToBrowse[].ReferenceTypeId`</span> | <a href="#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> | Reference type to follow.                                                             |
| <span class="pre">`NodesToBrowse[].BrowseDirection`</span> | <span class="pre">`ua.BrowseDirection`</span>                                                  | Browse direction.                                                                     |
| <span class="pre">`NodesToBrowse[].NodeClassMask`</span>   | <span class="pre">`ua.NodeClass`</span>                                                        | Node classes to include.                                                              |
| <span class="pre">`NodesToBrowse[].ResultMask`</span>      | <span class="pre">`ua.BrowseResultMask`</span>                                                 | Fields to include in the response.                                                    |
| <span class="pre">`NodesToBrowse[].IncludeSubtypes`</span> | boolean                                                                                        | Include subtypes of <span class="pre">`ReferenceTypeId`</span>.                       |

</div>

<div id="browseresult" class="section">

### BrowseResult<a href="#browseresult" class="headerlink" title="Link to this heading"></a>

| Field                                                   | Type                                                                                                         | Description                                  |
|---------------------------------------------------------|--------------------------------------------------------------------------------------------------------------|----------------------------------------------|
| <span class="pre">`Results[]`</span>                    | array                                                                                                        | One result per requested node.               |
| <span class="pre">`Results[].StatusCode`</span>         | StatusCode                                                                                                   | Browse status for the requested node.        |
| <span class="pre">`Results[].References[]`</span>       | array                                                                                                        | References returned by the browse operation. |
| <span class="pre">`References[].NodeId`</span>          | <a href="#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>               | Target node identifier.                      |
| <span class="pre">`References[].ReferenceTypeId`</span> | <a href="#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>               | Reference type identifier.                   |
| <span class="pre">`References[].IsForward`</span>       | boolean                                                                                                      | Reference direction.                         |
| <span class="pre">`References[].BrowseName`</span>      | <a href="#qualified-name-type" class="reference internal"><span class="std std-ref">QualifiedName</span></a> | Service name of the node.                    |
| <span class="pre">`References[].DisplayName`</span>     | <a href="#localized-text-type" class="reference internal"><span class="std std-ref">LocalizedText</span></a> | User-facing node label.                      |
| <span class="pre">`References[].NodeClass`</span>       | Int32                                                                                                        | Node class.                                  |
| <span class="pre">`References[].TypeDefinition`</span>  | <a href="#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a>               | Node type definition.                        |

</div>

<div id="closesecurechannelresponse" class="section">

### CloseSecureChannelResponse<a href="#closesecurechannelresponse" class="headerlink" title="Link to this heading"></a>

Empty table.

</div>

<div id="closesessionresponse" class="section">

### CloseSessionResponse<a href="#closesessionresponse" class="headerlink" title="Link to this heading"></a>

Empty table.

</div>

<div id="createsessionresponse" class="section">

### CreateSessionResponse<a href="#createsessionresponse" class="headerlink" title="Link to this heading"></a>

| Field                                            | Type                                                                                           | Description                                                  |
|--------------------------------------------------|------------------------------------------------------------------------------------------------|--------------------------------------------------------------|
| <span class="pre">`ResponseHeader`</span>        | table                                                                                          | Standard OPC UA response header.                             |
| <span class="pre">`SessionId`</span>             | <a href="#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> | Unique NodeId assigned by the server to the session.         |
| <span class="pre">`AuthenticationToken`</span>   | <a href="#node-id-type" class="reference internal"><span class="std std-ref">NodeId</span></a> | Unique token assigned by the server to the session.          |
| <span class="pre">`RevisedSessionTimeout`</span> | Double                                                                                         | Actual maximum session idle time in milliseconds.            |
| <span class="pre">`ServerNonce`</span>           | ByteString                                                                                     | Random value that should not be reused in another request.   |
| <span class="pre">`ServerCertificate`</span>     | ByteString                                                                                     | Server application instance certificate.                     |
| <span class="pre">`ServerEndpoints`</span>       | array                                                                                          | Endpoint descriptions supported by the server.               |
| <span class="pre">`ServerSignature`</span>       | table                                                                                          | Signature generated with the server certificate private key. |
| <span class="pre">`MaxRequestMessageSize`</span> | UInt32                                                                                         | Maximum request body size in bytes.                          |

</div>

<div id="findserversresponse" class="section">

### FindServersResponse<a href="#findserversresponse" class="headerlink" title="Link to this heading"></a>

| Field                                          | Type                                                                                                         | Description                              |
|------------------------------------------------|--------------------------------------------------------------------------------------------------------------|------------------------------------------|
| <span class="pre">`Servers[]`</span>           | array                                                                                                        | Servers that match the request criteria. |
| <span class="pre">`ApplicationUri`</span>      | string                                                                                                       | Application URI.                         |
| <span class="pre">`ProductUri`</span>          | string                                                                                                       | Product URI.                             |
| <span class="pre">`ApplicationName`</span>     | <a href="#localized-text-type" class="reference internal"><span class="std std-ref">LocalizedText</span></a> | Human-readable application name.         |
| <span class="pre">`ApplicationType`</span>     | number                                                                                                       | OPC UA application type.                 |
| <span class="pre">`GatewayServerUri`</span>    | string                                                                                                       | Gateway server URI, if used.             |
| <span class="pre">`DiscoveryProfileUri`</span> | string                                                                                                       | Discovery profile URI, if used.          |
| <span class="pre">`DiscoveryUrls`</span>       | string array                                                                                                 | Discovery endpoint URLs.                 |

</div>

<div id="getendpointsresponse" class="section">

### GetEndpointsResponse<a href="#getendpointsresponse" class="headerlink" title="Link to this heading"></a>

| Field                                          | Type       | Description                             |
|------------------------------------------------|------------|-----------------------------------------|
| <span class="pre">`Endpoints[]`</span>         | array      | Endpoint descriptions.                  |
| <span class="pre">`EndpointUrl`</span>         | string     | Endpoint URL.                           |
| <span class="pre">`Server`</span>              | table      | Server application description.         |
| <span class="pre">`ServerCertificate`</span>   | ByteString | Server certificate.                     |
| <span class="pre">`SecurityMode`</span>        | number     | Message security mode.                  |
| <span class="pre">`SecurityPolicyUri`</span>   | string     | Security policy URI.                    |
| <span class="pre">`UserIdentityTokens`</span>  | array      | Supported user identity token policies. |
| <span class="pre">`TransportProfileUri`</span> | string     | Transport profile URI.                  |
| <span class="pre">`SecurityLevel`</span>       | Byte       | Relative endpoint security level.       |

</div>

<div id="opensecurechannelresponse" class="section">

### OpenSecureChannelResponse<a href="#opensecurechannelresponse" class="headerlink" title="Link to this heading"></a>

| Field                                                    | Type     | Description                                 |
|----------------------------------------------------------|----------|---------------------------------------------|
| <span class="pre">`ResponseHeader`</span>                | table    | Standard OPC UA response header.            |
| <span class="pre">`SecurityToken`</span>                 | table    | Secure channel token.                       |
| <span class="pre">`SecurityToken.ChannelId`</span>       | UInt32   | Unique SecureChannel identifier.            |
| <span class="pre">`SecurityToken.TokenId`</span>         | UInt32   | Unique token identifier within the channel. |
| <span class="pre">`SecurityToken.CreatedAt`</span>       | DateTime | Token creation time.                        |
| <span class="pre">`SecurityToken.RevisedLifetime`</span> | UInt32   | Token lifetime in milliseconds.             |

</div>

<div id="readresponse" class="section">

### ReadResponse<a href="#readresponse" class="headerlink" title="Link to this heading"></a>

| Field                                | Type                                                                                                       | Description                              |
|--------------------------------------|------------------------------------------------------------------------------------------------------------|------------------------------------------|
| <span class="pre">`Results[]`</span> | <a href="#data-value-type" class="reference internal"><span class="std std-ref">DataValue</span></a> array | Attribute values returned by the server. |

</div>

<div id="writeresponse" class="section">

### WriteResponse<a href="#writeresponse" class="headerlink" title="Link to this heading"></a>

| Field                                | Type             | Description                                   |
|--------------------------------------|------------------|-----------------------------------------------|
| <span class="pre">`Results[]`</span> | StatusCode array | Results for the nodes written by the request. |

</div>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="application_certificate.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Application Certificate"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="interoperability_notes.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="OPC UA interoperability notes">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: opcua/wtf.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="index.html" class="icon icon-home">opcua-lua</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

- <a href="client/index.html" class="reference internal">Client</a>
- <a href="server/index.html" class="reference internal">Server</a>
- <a href="integrations/index.html" class="reference internal">Examples</a>
- <a href="model/index.html" class="reference internal">Address Space API</a>
- <a href="pubsub/index.html" class="reference internal">Publish Subscribe API</a>
- <a href="logging.html" class="reference internal">Logging subsystem</a>
- <a href="secure_policy.html" class="reference internal">Secure Policy</a>
- <a href="application_certificate.html" class="reference internal">Application Certificate</a>
- <a href="types.html" class="reference internal">Data Types</a>
- <a href="#" class="current reference internal">OPC UA interoperability notes</a>
  - <a href="#nodeid-representation-depends-on-the-encoding" class="reference internal">NodeId representation depends on the encoding</a>
  - <a href="#compact-binary-nodeids-are-encoding-choices" class="reference internal">Compact binary NodeIds are encoding choices</a>
  - <a href="#namespace-indexes-are-local-to-an-address-space" class="reference internal">Namespace indexes are local to an address space</a>
  - <a href="#nodeset2-xml-imports-require-namespace-remapping" class="reference internal">NodeSet2 XML imports require namespace remapping</a>
  - <a href="#practical-guidance" class="reference internal">Practical guidance</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[opcua-lua](index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="index.html" class="icon icon-home" aria-label="Home"></a>
- OPC UA interoperability notes
- <a href="_sources/wtf.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="opc-ua-interoperability-notes" class="section">

# OPC UA interoperability notes<a href="#opc-ua-interoperability-notes" class="headerlink" title="Link to this heading"></a>

OPC UA defines a common information model and service set, but the same concept can appear in different wire formats, files, and server address spaces. This page explains the parts that most often cause implementation mistakes and shows how to handle them in a predictable way.

<div id="nodeid-representation-depends-on-the-encoding" class="section">

## NodeId representation depends on the encoding<a href="#nodeid-representation-depends-on-the-encoding" class="headerlink" title="Link to this heading"></a>

A NodeId identifies a node within a namespace. The logical value is the same concept across OPC UA, but the encoded representation is not identical in every format.

For example, OPC UA JSON encoding and UADP/UA Binary encoding use different NodeId identifier type values:

| Identifier type                       | JSON value | UADP/UA Binary value |
|---------------------------------------|------------|----------------------|
| <span class="pre">`TwoByte`</span>    | not used   | 0                    |
| <span class="pre">`FourByte`</span>   | not used   | 1                    |
| <span class="pre">`Numeric`</span>    | 0          | 2                    |
| <span class="pre">`String`</span>     | 1          | 3                    |
| <span class="pre">`Guid`</span>       | 2          | 4                    |
| <span class="pre">`ByteString`</span> | 3          | 5                    |

The two lists look similar, but they are not the same enum. A JSON NodeId type must not be decoded with the UADP or UA Binary NodeId enum, and UADP or UA Binary values must not be decoded with the JSON enum.

There is an apparent numeric relationship for some values, such as <span class="pre">`UadpType`</span>` `<span class="pre">`=`</span>` `<span class="pre">`JsonType`</span>` `<span class="pre">`+`</span>` `<span class="pre">`2`</span> for Numeric, String, Guid, and ByteString. Treat this only as an observation, not as an implementation rule. It does not cover the compact TwoByte and FourByte binary forms and it makes the decoder depend on a shortcut instead of the active encoding.

</div>

<div id="compact-binary-nodeids-are-encoding-choices" class="section">

## Compact binary NodeIds are encoding choices<a href="#compact-binary-nodeids-are-encoding-choices" class="headerlink" title="Link to this heading"></a>

The binary encodings can represent some numeric NodeIds with compact forms:

| Binary form                         | When it can be used                                                                                                                                                                                  |
|-------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <span class="pre">`TwoByte`</span>  | Namespace <span class="pre">`0`</span> numeric identifiers in the range <span class="pre">`0`</span> to <span class="pre">`255`</span>.                                                              |
| <span class="pre">`FourByte`</span> | Namespace indexes in the range <span class="pre">`0`</span> to <span class="pre">`255`</span> and numeric identifiers in the range <span class="pre">`0`</span> to <span class="pre">`65535`</span>. |
| <span class="pre">`Numeric`</span>  | Larger namespace indexes and numeric identifiers.                                                                                                                                                    |

Do not infer the original binary NodeId form from the numeric identifier alone. The same logical NodeId can often be encoded in more than one valid binary form. A decoder should preserve the logical NodeId value. An encoder may choose the smallest valid binary form unless a specific format is required by the caller or by an interoperability test.

Recommended implementation pattern:

- Decode each transport format with its own format-specific parser.

- Convert decoded values into one internal NodeId representation.

- Encode from that internal representation using the target wire format.

- Keep JSON, UADP, and UA Binary enum handling separate.

</div>

<div id="namespace-indexes-are-local-to-an-address-space" class="section">

## Namespace indexes are local to an address space<a href="#namespace-indexes-are-local-to-an-address-space" class="headerlink" title="Link to this heading"></a>

A NodeId contains a namespace index. That index is not a global identifier. It is an index into the server’s namespace table, so the same companion specification can have different indexes on different servers.

For example, two servers may expose the same type of machine and the same companion specification, but load NodeSet2 XML files in a different order. The address-space hierarchy may be equivalent while the namespace indexes in NodeIds are different.

Applications should avoid treating namespace indexes as stable across servers. Use one of these strategies instead:

- Resolve the namespace URI from the server namespace table before comparing or storing NodeIds outside the current server connection.

- Store external references as ExpandedNodeIds or as <span class="pre">`namespaceUri`</span> plus identifier.

- Use browse paths or <span class="pre">`TranslateBrowsePathsToNodeIds`</span> when a stable model path is more appropriate than a hard-coded NodeId.

- Load companion specifications in a deterministic order when you control the server configuration.

</div>

<div id="nodeset2-xml-imports-require-namespace-remapping" class="section">

## NodeSet2 XML imports require namespace remapping<a href="#nodeset2-xml-imports-require-namespace-remapping" class="headerlink" title="Link to this heading"></a>

NodeSet2 XML files contain a namespace table and NodeIds that refer to entries in that table. The namespace indexes inside a file are local to that file. When the file is imported into a server model, those indexes must be mapped to the indexes assigned by the target address space.

The default OPC UA namespace has index <span class="pre">`0`</span>. Servers also commonly reserve a local application namespace, often index <span class="pre">`1`</span>. Imported companion specifications are then assigned indexes according to the model loader’s namespace table.

Correct import logic must therefore:

- Read the namespace URIs declared by the XML file.

- Create or find the corresponding namespaces in the target model.

- Rewrite NodeIds, reference targets, type definitions, data type references, and aliases from XML-local indexes to target-model indexes.

- Preserve the namespace URI as the stable identity of the namespace.

This remapping is expected OPC UA behavior, not a data corruption issue. A NodeId string copied directly from a NodeSet2 file may not be valid in the same form after import unless the namespace index happens to be the same.

</div>

<div id="practical-guidance" class="section">

## Practical guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

For robust OPC UA applications:

- Treat NodeIds as structured values, not as plain strings.

- Treat namespace URI as the stable namespace identity.

- Use namespace indexes only after resolving them in the current server or model.

- Keep transport-specific encoding code separate from model-level NodeId logic.

- Prefer browse paths or well-known type definitions when connecting to servers that may load companion specifications in different orders.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="types.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Data Types"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, Real Time Logic.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>
