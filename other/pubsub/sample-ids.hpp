#ifndef SAMPLE_IDS_HPP
#define SAMPLE_IDS_HPP

#define SAMPLE_SERVICE_ID           0x1001
#define SAMPLE_INSTANCE_ID          0x2001
#define SAMPLE_METHOD_ID            0x5001  // must less than 32766 (0x7FFE)

#define SAMPLE_EVENT_ID             0x8001   // must greater than 32767(0x7FFF)
#define SAMPLE_GET_METHOD_ID        0x0001
#define SAMPLE_SET_METHOD_ID        0x0002

#define SAMPLE_EVENTGROUP_ID        0x6001

#define OTHER_SAMPLE_SERVICE_ID     0x3001
#define OTHER_SAMPLE_INSTANCE_ID    0x4001
#define OTHER_SAMPLE_METHOD_ID      0x5100

#endif  // SAMPLE_IDS_HPP