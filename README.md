# Simple arduino communication system. 
It allows user to comunicate with ARDUINO UNO through simple prompts using python terminal. 
For now it work only for communication without robot actions. 

# Commands:
`help` - shows list of commands
`save` - saving logs to .txt file
`history` - showing logs history from last save
`M<number>` - robot drives <number> of cm
`V<number>` - setting constant speed
`R<number>` - rottating robot by provided angle
`E` - changing engine settings
`X` - getting robot status
`I` - IR data
`B` - sonar data

# Communcation example:

Message sent: `M100`
Message recevied: `ACK, moving`

Message sent: `ALA MA KOTA` (bad request intentionally)
Message recevied: `NACK, wrong message`

# Communcation safety
Communication safety is ensured by check sum calculated using `XOR` operation calculated from sent message. 
Arduino checks checksum in order to make sure that message wasn't lost in communication tunels.
Communcation system also uses exponential backoff, in case of no response, and will try to send message 3 times before shutdown

