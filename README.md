# PolyNetClient
A TCP client with a compile time randomly generated networking protocol.  
  
This is a technique used by games such as RuneScape, and Leage of Legends to prevent tampering of their networking (and in the larger scheme of things, to prevent headless botting.) This works best when combined with continuous integration, so that updates are always being pushed out with new networking characteristics. Improvements to this could include randomization of the few exposed implementation details, and possibly transferring encoding details over the network.
