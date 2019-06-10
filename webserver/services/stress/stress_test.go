package stress_test

import (
	"fmt"
	"log"
	"testing"
	"time"

	"github.com/AramisHM/Domefy/webserver/services/stress"
)

func TestRestStressTest(t *testing.T) {
	start := time.Now()

	for i := 0; i < 144; i++ {
		stress.RestStressTest()
	}

	elapsed := time.Since(start)
	log.Printf("Binomial took %s", elapsed)
	fmt.Println("ok")
}
