<?lsp

--[[
This example shows how to use the LSP App Manager's integrated Let's
Encrypt plugin in automatic mode.

Simply run this file using the LSP App Manager and follow the
instructions.

The logic for managing the two modules acmebot and acmedns would
normally not be crammed into one LSP page, but we do it this way in
order to deliver one single example file. Most of the logic would
normally be put in a .preload/.config file or in a dedicated
module. This LSP page uses resources from
https://tutorial.realtimelogic.com/ as a CDN repository and will not
function unless your browser can connect to this server.

This LSP page includes state management and the persistent state
information is stored in the persistent "page" table.
--]]

local function decodeErr(err)
   if err:find("cannotresolve",1,true) or 
      err:find("cannotconnect",1,true) then
      err = "Server cannot connect to the Internet"
   end
   return err
end

local function loadSecurityModule()
   -- Load the 'embedded' version of the security module 'tokengen'
   pcall(function() page.zonename=require"etokengen".info() end)
   return  page.zonename
end

local function emitIsSecure() ?>
<details open>
<summary style="background:#11772d">You are using an Encrypted Connection</summary>
<p><br/></p>
<h4>Cipher Suite: <?lsp=request:cipher()?></h4>
<p>A secure connection and an encrypted connection are not the same. Is this connection secure? Find out by reading the <a href='https://makoserver.net/articles/Lets-Encrypt'>full article</a>.</p>
</details>
<?lsp end
local function emitAcmeActive(dn)
-----------------------------------------------------------
?>
<details open>
<summary>Certificate Installed</summary>
<p>The Let's Encrypt signed certificate is installed and you may navigate to the secure URL by navigating to:</p>
<h3><a target="_blank" href="https://<?lsp=string.format("%s:%d%s",dn,ba.serversslport,request:uri())?>">https://<?lsp=dn?></a></h3>
<p><b>Note:</b>the DNS translation to IP address fails if you have DNS rebinding protection enabled in your router or enabled by your DNS provider.</p>

<?lsp if not page.revcon then ?>
<h2>Remote Access:</h2>
<p>The Reverse Connection module enables optional remote server access via the <a target="_blank" href="https://realtimelogic.com/products/SharkTrustX/">Remote Access Bridge</a>, an integrated SharkTrustX plugin. Makes sure to read up on the <a target="_blank" href="https://makoserver.net/articles/Secure-Remote-Access">security implications</a> before enabling this feature!</p>
<form method="post">
<input type="submit" name="revcon" value="Enable Reverse Connection"/>
</form>
<?lsp else ?>
<strong>Remote access via <?lsp=string.format('<a target="_blank" href="https://%s/login.html">%s</a>', page.zonename,page.zonename)?> enabled.</strong>
<?lsp end ?>
</details>
<?lsp end
local function emitWaiting()
-----------------------------------------------------------
?>
<details open>
<summary>Communicating with Let's Encrypt</summary>
<div class="spinnerc">
<svg class="spinner" viewBox="0 0 50 50">
  <circle class="path" cx="25" cy="25" r="20" fill="none" stroke-width="5"></circle>
</svg>
</div>
<p id='info'>The server is communicating with Let's Encrypt and the online DNS service. Setting up the ACME DNS TXT record takes more than two minutes. Feel free to read the <a target="_blank" href='https://makoserver.net/articles/Lets-Encrypt'>full article</a> while waiting.</p>
<script>
$(function() {
    $('#info').hide();
    function busy() {
        $.getJSON(window.location.href,
                  function(rsp) {
                      if(rsp.busy)
                      {
                          $('#info').show();
                          setTimeout(busy, 2000);
                      }
                      else
                          location=location.href;
                  });
    }
    setTimeout(busy, 2000);
});
</script>
</details>
<?lsp end
local function emitRegister(error,email)
-----------------------------------------------------------'
if not page.agreementURL then
   -- Fetch Let's Encrypt's terms (PDF) link.
   page.agreementURL=require"acme/engine".terms()
end
?>

<details open>
<summary>Install Trusted Certificate</summary>
<?lsp
if error then
   response:write("<p class='err'>"..error)
else ?>
<p>This server comes with an integrated <a target="_blank" href="https://realtimelogic.com/products/SharkTrustX/">SharkTrustX</a> security module for the zone <b><?lsp=page.zonename?></b>.</p>

<p>Simply enter your email address below to install a trusted <a target='blank' href='https://letsencrypt.org/'>Let's Encrypt</a> signed certificate for your private server.</p>
<?lsp end ?>

<form method="post">
<table class="iw">
<tr><td>E-Mail Address:</td><td><input type="text" name="email" placeholder="The e-mail address sent to Let's Encrypt" value="<?lsp=email or""?>"/></td></tr>
</table>
<input type="submit" value="Submit"/> 
</form>
<p><b>Note:</b> You accept the <a target="_blank" href="<?lsp=page.agreementURL?>">Let's Encrypt Subscriber Agreement</a> by clicking the above submit button.</p>
</details>
<?lsp end
local function emitNoSecurityModule()
?>
<details open>
<summary>Security Module Not Installed</summary>

<p>This example requires a server with an installed security module. You can install (compile and integrate) a security module as explained in the <a href="http://realtimelogic.com/ba/doc/?url=Mako.html#securitymodule">Mako Server's Security Module section</a>.</p>
</details>

<?lsp end ?>

<!DOCTYPE html>
<html>
<head>
   <meta charset="UTF-8"/>
   <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>SSL/TLS &amp; Trust</title>
  <link rel="stylesheet" href="https://tutorial.realtimelogic.com/intro-style.css" />
  <link rel="shortcut icon" href="https://sharktrustx.realtimelogic.com/favicon.ico" />
  <script src="/rtl/jquery.js"></script>
</head>
<body>
<div id="mainContainer">
  <div id="header">
    <img src="https://tutorial.realtimelogic.com/images/BAS-logo.png" />
    <div id="headertxt">TLS: It's All About the Trust</div>
  </div>
<div id="leftSidebar">
<div style="writing-mode: vertical-rl;margin:130px auto">
<h2>Example source code is a modified copy of:</h2>
<h2><a href="https://tutorial.realtimelogic.com/SharkSSL.lsp">https://tutorial.realtimelogic.com/SharkSSL.lsp</a></h2>
</div>
</div>
  <div id="mainContent">
  <div class="innertube">
 <p>Trust is primary, encryption is secondary. <a href='#' id='why'>Why?</a></p>
<?lsp

local acmebot=require"acme/bot"
if request:header"x-requested-with" then -- AJAX from $.getJSON() above
   local jobs=acmebot.status()
   response:json{busy=jobs > 0 and true or false}
end

if request:issecure() then
   emitIsSecure()
else
   if not page.zonename and not loadSecurityModule() then
      emitNoSecurityModule()
   else
      local op={
         info="LSP Application Manager's Let's Encrypt example",
         acceptterms=true,
         rsa=true,
         --production=false -- Enable staging
      }
      local acmedns=require"acme/dns"
      local email,domain=acmedns.auto(op) -- Starts engine if previosly registered
      local jobs,_,lastError = acmebot.status(true)
      if request:method() == "POST" then
         local d={} -- Extract email from form data and use 'name@' as the sub domain name (excluding @)
         for k,v in pairs(request:data()) do d[k]=v:gsub("^%s*(.-)%s*$", "%1") end -- trim start/end
         if d.revcon and acmedns.active()=="auto" then
            op.revcon=true
            acmedns.auto(op)
            page.revcon=true
            emitAcmeActive(domain)
         else
            local email email=d.email
            if email and #email > 8 and email:find'@' and email:find'@' > 3 then
               local dn = email:match"[^@]+"
               acmedns.auto(email, dn:gsub("[^%w]",""), op) -- Activate acme/dns
               emitWaiting()
            else
               emitRegister"Invalid email address"
            end
         end
      elseif jobs > 0 then
         emitWaiting()
      elseif lastError then
         emitRegister(decodeErr(lastError))
      elseif acmedns.active()=="auto" then
         emitAcmeActive(domain)
      else
         emitRegister()
      end
   end
end
?>

<script>
$(function() {
    $("#why").click(function() {
        console.log("why");
        $(this).parent().after("<p>Because TLS is rendered useless when the certificate is not trusted and when the user is forced to bypass the browser security in order to access the server. The user cannot differentiate between a man in the middle and the non trusted server's certificate. See the article <a href='https://realtimelogic.com/articles/How-Anyone-Can-Hack-Your-Embedded-Web-Server'>Anyone can hack your HTTPS Server</a> for more details.</p>");
        $(this).replaceWith("Why?");
        return false;
    });
});
</script>

<details>
<summary>What is Trust?</summary>
<p>The Barracuda App Server includes <a href='https://realtimelogic.com/products/sharkssl/'>SharkSSL</a> and just like any other TLS stack, SharkSSL supports the two required TLS encryption technologies:</p>
<ol>
<li>Asymmetric Encryption (RSA or ECC)</li>
<li>Symmetric Encryption (AES-GCM, ChaCha20-Poly1305, etc)</li>
</ol>
<p>Trust is established during the initial asymmetric encryption, which is used for deriving keys used by symmetric encryption. The browser validates the server's certificate as part of the asymmetric encryption handshaking. The following figure shows how the server sends its certificate to the browser.</p>
<div class="center"><img src="https://tutorial.realtimelogic.com/images/ssl-trust.gif" alt="SSL"/></div>
<p>The browser does not trust the server's certificate unless: (1) it has a hard-copy of the Certificate Authority (CA) certificate that was used to sign the server's certificate, and (2) the domain name in the URL matches any of the names in the server's certificate.</p>
See the tutorial <a href='https://realtimelogic.com/articles/Certificate-Management-for-Embedded-Systems'>Introduction to asymmetric algorithms</a> for details.
</details>
<details>
<summary>How to Trust the Included (Embedded) Certificate</summary>
<p>The server automatically installs a default certificate, thus you may change the URL from http:// to https:// at any time. The default certificate is not trusted by your browser, but you can establish trust by installing the Certificate Authority (CA) certificate that was used to sign the server's default certificate.</p>
<p><b>Establish trust, using the default certificate, by making sure the two following conditions are met:</b></p>
<ol>
<li>Install <a href="https://realtimelogic.com/downloads/root-certificate/">Real Time Logic's CA certificate</a>
<li>Make sure your browser URL matches any of the names listed in the certificate.</li>
</li>
</ol>
<p>The default certificate is a so called SAN certificate and includes the following (domain) names: localhost, 127.0.0.1, and MakoServer. Use the name localhost if the server and browser are on the same computer; otherwise set up an entry in your <a target="_blank" href="https://en.wikipedia.org/wiki/Hosts_(file)"> hosts file</a> for the name MakoServer.</p>
</details>
<details>
<summary>How to Create Your Own Chain of Trust</summary>
<p>You may use the <a href="certmgr.lsp">Certificate Management App</a> to create a complete chain of trust, including the CA certificate, and any number of server certificates. Note that none of the created certificates will be trusted by any browser unless you install the CA certificate in all browsers intended to access the server.</p>
</details>

</div></div></div>
</body></html>
