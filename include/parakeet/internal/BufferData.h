/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_BUFFERDATA_H
#define PARAKEET_BUFFERDATA_H

namespace mechaspin
{
    namespace parakeet
    {
        namespace internal
        {
            struct BufferData
            {
                unsigned char* buffer;
                unsigned int length;

                BufferData()
                {
                    //Intentionally left blank
                }

                BufferData(unsigned char* buffer, unsigned int length)
                {
                    this->buffer = buffer;
                    this->length = length;
                }
            };
        }
    }
}

#endif