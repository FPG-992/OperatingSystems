
/*
    Θεωρια

    Σήμα (signal) ειναι ενας τροπος επικοινωνιας των διεργασιων. Ειναι μια ειδοποιηση
    που στελνεται σε μια διεργασια προκειμενου να την ενημερωσει για καποιο γεγονος.

    Κάθε σήμα μπορεί να έχει έναν χειριστή σήματος (handler), ο οποίος είναι μια
    συνάρτηση που εκτελείται όταν η διεργασία λαμβάνει το συγκεκριμένο σήμα. Η
    συνάρτηση αυτή καλείται ασύγχρονα.
    Όταν στέλνεται το σήμα σε μια διεργασία, το ΛΣ σταματά την εκτέλεση της διεργασίας,
    και "την αναγκάζει" να καλέσει την αντίστοιχη συνάρτηση χειρισμού αυτού του σήματος
    (handler).
    Όταν η συνάρτηση ολοκληρώσει την εκτέλεσή της, η διεργασία συνεχίζει την εκτέλεση
    από το σημείο που βρισκόταν αμέσως πριν παραληφθεί το σήμα.
*/

/*
    Εκφωνηση

    Να γραφτει προγραμμα στο ο ποιο η διεργασια-πατερασ (F) δημιουργει 'n' διεργασιες-
    παιδια (C1, C2, ..., Cn).
    Ο χρηστης θα μπορει να στελνει σηματα SIGUSR1, SIGUSR2, SIGTERM στην διεργασια-πατερα
    και στις διεργασιες παιδα.

    Παραδειγμα εκτελεσης: ./gates tfttt

    Το προγραμμα μπορει να χρησιμοποιηθει για τη διαχειριση ενος αριθμου πυλων (gates).
    Ο πατερας δημιουργει Ν διεργασιες (το Ν ειναι το μηκος του στρινγ "tfttt"). Καθε μια
    διεργασια παιδι θα 'διεχειριζεται' την αντιστοιχη πυλη.
    Ο πατερας δεχεται τον αριθμο των πυλων και την αρχικη τους κατασταση ως συμβολοσειρα
    που αποτελειται απο χαρακτηρες "t", η "f", πχ "ttfff" σημαίνει 5 πύλες, 2 ανοιχτες
    και 3 κλειστες.

    Αρχικά, η διεργασια πατερας δημιουργει ολες τις διεργασιες παιδια και τυπωνει το
    δεντρο διεργασιων που εχει δημιουργηθει ωστε να φαινονται τα PID τους:
        [PARENT/PID=389] Created child 0 (PID=390) and initial state 't'
        [PARENT/PID=389] Created child 1 (PID=391) and initial state 't'
        [PARENT/PID=389] Created child 2 (PID=392) and initial state 'f'

    Οι διεργασιες παιδια καθε 15 δευτερολεπτα τυπωνουν το state τους (με alarm) και τον
    χρονο που εκτελουνται.
        [ID=0/PID=490/TIME=0s] The gates are open!
        [ID=1/PID=490/TIME=0s] The gates are open!
        [ID=2/PID=490/TIME=0s] The gates are close!
        ...
        [ID=0/PID=490/TIME=105s] The gates are open!
        [ID=1/PID=490/TIME=105s] The gates are open!
        [ID=2/PID=490/TIME=105s] The gates are close!
        [ID=0/PID=490/TIME=120s] The gates are open!
        [ID=1/PID=490/TIME=120s] The gates are open!
        [ID=2/PID=490/TIME=120s] The gates are close!
        ...
    
    Οι διεργασιες παιδια:
        - οταν λαμβανουν σημα SIGUSR1 τυπωνουν το state τους (την κατασταση της πυλης),
            και τα δευτερλεπτα απο τηβ στιγμη εκκινησης.
        - οταν λαμβανουν σημα SIGUSR2 κανουν flip το state.
        - οταν λαμβανουν σημα SIGTERM τερματιζουν.
    Μπορουν να λαμβανουν σημα ειτε απο τον χρηση μεσω terminal, ειτε απο τον πατερα.

    Η διεγασια πατερας:
        - οταν λαβει σημα SIGUSR1, στελνει SIGUSR1 σε ολα τα παιδια.
        - οταν λαβει σημα SIGTERM, στελνει SIGTERM σε ολα τα παιδια.
        - φροντιζει οτι ολα τα παιδια ειναι υγιη, οποτε ελεγχει για σηματα SIGCHLD και:
            -- αν καποιο παιδι εχει σκοτωθει, το κανει wait και φτιαχνει καινουργιο
                στη θεση του.
            -- αν καποιο παιδι εχει σταματησει, το κανει resume.
        Θα πρεπει να υπαρχουν παντα Ν παιδια ενεργα, ενα για καθε πυλη.
    Η διεργασια πατερας μπορει να λαβει σημα απο τον χρηστη με μεσω terminal.

    Μια υποδειγματικη εκτελεση του προγραμματος:
    ./gates ft
    [PARENT/PID=100] Created child 0 (PID=101) and initial state 'f'
    [PARENT/PID=100] Created child 1 (PID=102) and initial state 't'
    [ID=0/PID=101/TIME=0s] The gates are closed!
    [ID=1/PID=102/TIME=0s] The gates are open!
    [ID=0/PID=101/TIME=15s] The gates are closed!
    [ID=1/PID=102/TIME=15s] The gates are open!
    kill -SIGUSR2 102
    [ID=1/PID=102/TIME=24s] The gates are closed!
    [ID=0/PID=101/TIME=30s] The gates are closed!
    [ID=1/PID=102/TIME=30s] The gates are closed!
    kill -SIGTERM 101
    [PARENT/PID=100] Child 0 with PID=101 exited
    [PARENT/PID=100] Created new child for gate 0 (PID 103) and initial state 'f'
    [ID=0/PID=103/TIME=0s] The gates are closed!
    [ID=1/PID=102/TIME=45s] The gates are closed!
    [ID=0/PID=103/TIME=15s] The gates are closed!
    [ID=1/PID=102/TIME=60s] The gates are closed!
    [ID=0/PID=103/TIME=30s] The gates are open!
    kill -SIGRΤERM 100
    [PARENT/PID=100] Waiting for 2 children to exit
    [PARENT/PID=100] Child with PID=103 terminated successfully with exit status code 0!
    [PARENT/PID=100] Waiting for 1 children to exit
    [PARENT/PID=100] Child with PID=102 terminated successfully with exit status code 0!
    [PARENT/PID=100] All children exited, terminating as well
*/

/*
    Υποδειξεις

    1. Ελεγχος ορθοτητας ορισματων (argc, argv) και εκτυπωση καταλληλου μηνυματος.
    2. Ελεγχος σφαλματων για καθε κληση συστηματος.
    3. Ο κωδικας των παιδιων ειναι σε ξεχωριστο αρχειο child.c (να γινει χρηση execv).
    4. Να δημιουργει makefile που παραγει τα εκτελεσιμα.
*/

/*
    Εργαλεια

    Μια διεργασία μπορεί να στείλει ένα σήμα σε μια άλλη, με την κλήση kill(pid, SIGNAL)
    η οποία στέλνει το σήμα με όνομα SIGNAL στη διεργασία με process id ίσο με το όρισμα
    pid.
    Η εντολή αποστολής ενός σήματος από terminal είναι: 'kill -SIGNAL pid'.

    Η waitpid() χρησιμοποιείται για να περιμένει η καλούσα διεργασία αλλαγές κατάστασης
    σε μια διεργασία παιδί της και να λάβει πληροφορίες σχετικά με την διεργασία της
    οποίας η κατάσταση έχει αλλάξει.
        'pid_t waitpid(pid_t pid, int *wstatus, int options)'
    - Το πεδίο pid ορίζει το pid της διεργασίας που περιμένουμε να αλλάξει κατάσταση
        (με τιμή ίση με -1 περιμένουμε οποιαδήποτε διεργασία παιδί).
    - Το πεδίο wstatus είναι ο δείκτης στην μεταβλητή στην οποία μπορούμε να αποθηκεύσουμε
        την κατάσταση της διεργασίας παιδί.
    - Το πεδίο options ορίζει επιπλέον επιλογές.
    Περισσοτερες πληροφοριες στον man-page της εντολης.

    Μια διεργασία μπορεί να ορίσει τον χειριστή (handler) κάθε σήματος που θα λάβει με
    τις συναρτήσεις: sigaction() (προτεινεται), singal().
    - Η κλήση signal(SIGNAL, handler) καθορίζει ότι όταν η καλούσα διεργασία λάβει σήμα
        με όνομα SIGNAL θα εκτελέσει τη συνάρτηση χειρισμού σήματος handler.
    - Η κλήση συστήματος sigaction () χρησιμοποιείται για να αλλάξει τη δράση (action)
        που θα γίνει από μια διεργασία κατά την λήψη συγκεκριμένου σήματος.
        'int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)'
        -- Η παράμετρος signum καθορίζει το σήμα και μπορεί να είναι οποιοδήποτε έγκυρο
            σήμα εκτός από SIGKILL και SIGSTOP.
        -- Εάν η παράμετρος act είναι not NULL τότε θέτει το νέο sigaction για το δεδομένο
            signal.
        -- Εάν η παράμετρος oldact είναι not NULL τότε εκεί αποθηκεύεται η προηγούμενη
            ορισμένη δράση για το δεδομένο signal.
        -- Η παράμετρος act και oldact είναι δομές τύπου struct sigaction:
            struct sigaction {
                void (*sa_handler)(int);                         // καθορίζει τη δράση που πρέπει να συσχετιστεί με το signum
                void (*sa_sigaction)(int, siginfo_t *, void *);  // εάν ορίζεται SA_SIGINFO στο sa_flags, τότε στο sa_sigaction (αντί του sa_handler) καθορίζεται η λειτουργία χειρισμού για το signum μην εκχωρήσετε τιμή και στα δύο: sa_handler και sa_sigaction
                sigset_t sa_mask;                                // καθορίζει μια μάσκα σημάτων που πρέπει να αποκλειστούν
                int sa_flags;                                    // καθορίζει ένα σύνολο σημαιών που τροποποιούν τη συμπεριφορά του σήματος
                void (*sa_restorer)(void);                       // το πεδίο δεν προορίζεται για application use
            };

            Παραδειγμα βασικης χρησης sigaction():
                struct sigaction action;            // δήλωση δομής struct sigaction
                action.sa_handler = myfunction;     // ορισμός ονόματος συνάρτησης χειρισμού
                sigaction(SIGUSR1, &action, NULL);  // ορισμός χειριστή (myfunction) για το σήμα SIGUSR1
            Αποτέλεσμα: όταν η διεργασία λάβει το σήμα SIGUSR1 θα διακόψει την εκτέλεση της
            και θα καλέσει την συνάρτηση myfunction().
            Περισσοτερες πληροφοριες στο man-page της εντολης.
    
    Οι διεργασίες μπορούν να ορίσουν να εκτελούν μια εργασία μετά την πάροδο συγκεκριμένου
    χρόνου. Για να το πετύχουν αυτό οι διεργασίες μπορούν να κάνουν κλήση της συνάρτησης
    alarm().
    Η συνάρτηση alarm(unsigned int seconds) θα προκαλέσει το σύστημα να παράγει ένα σήμα
    SIGALRM, προς τη διεργασία που την κάλεσε, μετά την παρέλευση του αριθμού των
    δευτερόλεπτων πραγματικού χρόνου που καθορίζονται από την παράμετρο seconds.

    Χρησιμη αναφορα: https://man7.org/linux/man-pages/man7/signal.7.html
*/
