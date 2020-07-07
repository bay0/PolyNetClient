# PolyNetClient
A TCP client with a compile time randomly generated networking protocol.  
  
This is a technique used by games such as RuneScape, and Leage of Legends to prevent tampering of their networking (and in the larger scheme of things, to prevent headless botting.) This works best when combined with continuous integration, so that updates are always being pushed out with new networking characteristics. Improvements to this could include randomization of the few exposed implementation details, and possibly transferring encoding details over the network.

## BE
Other improvments could also be made in regards to very specific use cases, suh as BE. BE sends challenge packets to the client, and requires them to generate a valid, 
seemingly random response in order to stay connected. This type of data should be obfuscated, and randomized, along with the rest of the networking. In no circumstance 
should this challenge contain static data.
