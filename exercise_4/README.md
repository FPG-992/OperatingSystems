Θα πρέπει να υλοποιήσετε έναν client που
επικοινωνεί με δικό μας server μέσω sockets.
Μέσω της επικοινωνίας θα μεταφέρονται και θα
εγκρίνονται αιτήματα μετακίνησης και θα
παρέχονται πληροφορίες αναφορικά με την
τρέχουσα θερμοκρασία κα.

./ask4 [--host HOST] [--port PORT] [--debug]
Το πρόγραμμα είναι εφικτό να δεχθεί τα παρακάτω
ορίσματα κατά την εκτέλεση
--host HOST: Συνδέεται στο HOST αντί για το default.
--port PORT: Συνδέεται στο PORT αντί για το default
--debug: Εκτυπώνει σε μήνυμα τα δεδομένα που έχουν
αποσταλεί ή ληφθεί
Default: HOST os4.iot.dslab.ds.open-cloud.xyz
PORT 20241

Εντολές από terminal:
help : Εκτυπώνει help message
exit : Έξοδος
get : Ανάκτηση δεδομένων server
N name surname reason : Άδεια εξόδου στην καραντίνα

Παράδειγμαget:
get
[DEBUG]sent 'get‘
[DEBUG] read '2 058 2950 1589989296‘
---------------------------
Latest event:
interval (2)
Temperature is: 29.50
Light level is: 58
Timestamp is: 2020-05-2018:41:36 // (use localtime() )

Παράδειγμαget:
Ο server θα απαντήσει με τις πιο πρόσφατες μετρήσεις
αισθητήρα στην ακόλουθη μορφή
X YYY ZZZZ WWWWWWWWWW, όπου
YYY είναι η φωτεινότητα
ZZZZ είναι η θερμοκρασία (χρειάζεται διαίρεση με 100)
WWWWWWWWWW είναι UNIX timestamp και
X είναι ένας ακέραιος αριθμός που δείχνει τον τύπο συμβάντος,
σύμφωνα με το παρακάτωπρωτόκολλο
0: boot , 1: setup, 2: interval, 3: button, 4: motion

Παράδειγμαget:
get
[DEBUG]sent 'get‘
[DEBUG] read '2 058 2950 1589989296‘
---------------------------
Latest event:
interval (2)
Temperature is: 29.50
Light level is: 58
Timestamp is: 2020-05-2018:41:36 // (use localtime() )

ΠαράδειγμαN name surname reason :
1 aggelos kolaitis dokimi
[DEBUG]sent '1 aggelos kolaitis dokimi'
[DEBUG] read '5fdd09689ffe'
Send verification code: '5fdd09689ffe'
5fdd09689ffe
[DEBUG]sent '5fdd09689ffe'
[DEBUG] read 'ACK 1 aggelos kolaitis dokimi'
Response: 'ACK 1 aggelos kolaitis dokimi‘
Στην περίπτωση λανθασμένου μηνύματος ο server απαντάει
'try again

Μια διεργασία είναι εφικτό να δεχθεί δεδομένα από
ένα σύνολο file descriptors ή από το terminal, μέσω της select().
H select() επιτρέπει σε ένα πρόγραμμα να παρακολουθεί πολλούς
περιγραφητές αρχείων, έως ότου ένας ή περισσότεροι από τους
περιγραφητές αρχείων γίνουν «έτοιμοι». Ένας περιγραφητής
θεωρείται έτοιμος εάν είναι δυνατή η εκτέλεση μιας λειτουργίας
εισόδου / εξόδου (π.χ. ανάγνωση ή έγγραφή) χωρίς αποκλεισμό.
Περισσότερες πληροφορίες:
• http://man7.org/linux/man-pages/man2/select.2.html
• αρχείο mario.c στο στον φάκελο Χρήσιμα Αρχεία της Άσκησης 4

http://man7.org/linux/man-pages/man2/socket.2.html
