import cv2
import serial
from deepface import DeepFace

# To capture video from webcam
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
cap.set(cv2.CAP_PROP_FPS, 30)


def check_faces(frame):
    
    memberList = "000"

    try:
        results = DeepFace.find(
            frame,
            "./agent1_host/database",
            enforce_detection=False,
            silent=True,
            threshold=0.5,
        )

        for result in results:
            if not result.empty:
                # print(f"{result.iloc[0]}\n")
                if result.iloc[0]["distance"] < 0.35:
                    identity = result.iloc[0]["identity"].split("\\")[-2]
                    print(f"=== detect {identity}")
                    
                    if identity == 'english':
                        memberList = '1' + memberList[1:]
                    elif identity == 'fish':
                        memberList = memberList[0] + '1' + memberList[2]
                    else:
                        memberList = memberList[:2] + '1'

                    # target_x, target_y, target_w, target_h = (
                    #     result["target_x"],
                    #     result["target_y"],
                    #     result["target_w"],
                    #     result["target_h"],
                    # )

                    # cv2.putText(
                    #     frame,
                    #     f"${result['identity']}",
                    #     (target_x, target_y),
                    #     cv2.FONT_HERSHEY_SIMPLEX,
                    #     1,
                    #     (255, 0, 0),
                    #     3,
                    # )

                    # cv2.rectangle(
                    #     frame,
                    #     (target_x, target_y),
                    #     (target_x + target_w, target_y + target_h),
                    #     (255, 0, 0),
                    #     2,
                    # )

    except ValueError:
        pass

    print(memberList)
    # serial.write(bytes("1".encode('ascii')))
    return frame


# serial = serial.Serial('COM4', 9600, timeout=0)
counter = 0

while True:
    ret, frame = cap.read()

    if ret:
        if counter % 30 == 0:
            counter = 0
            try:
                frame = check_faces(frame.copy())
            except ValueError:
                pass

        counter += 1
        cv2.imshow("img", frame)

    # Stop if escape key is pressed
    key = cv2.waitKey(30) & 0xFF
    if key == 27:
        break

cap.release()
